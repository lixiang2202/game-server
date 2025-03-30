#pragma once
#include "base/time_helper.h"
#include "base/object_pool.h"
#include "logging.h"

#include <array>
#include <map>
#include <memory>
#include <unordered_map>
#include <chrono>
#include <bitset>
#include <thread>

// 第一个轮占的位数
constexpr int8_t FIRST_WHEEL_BITS = 2;
// 第一个轮的长度
constexpr int32_t FIRST_WHEEL_SIZE = 1 << FIRST_WHEEL_BITS; // 256
// 其他轮占的位数
constexpr int8_t OTHER_WHEEL_BITS = 6;
// 其他轮的长度
constexpr int32_t OTHER_WHEEL_SIZE = 1 << OTHER_WHEEL_BITS; // 64
// 其他轮数量
constexpr int8_t OTHER_WHEEL_COUNT = (32 - FIRST_WHEEL_BITS) / OTHER_WHEEL_BITS; // 4;
// 总共的时间轮数量
constexpr int8_t WHEEL_COUNT = OTHER_WHEEL_COUNT + 1; // 5

// 第一个轮掩码
constexpr uint32_t FIRST_WHEEL_MASK = FIRST_WHEEL_SIZE - 1; // 0b11111111
// 其他轮掩码
constexpr uint32_t OTHER_WHEEL_MASK = OTHER_WHEEL_SIZE - 1; // 0b111111

// 第一个轮的当前位置
constexpr inline uint32_t FIRST_WHEEL_INDEX(uint32_t v) { return v & FIRST_WHEEL_MASK; }
// 其他轮的当前位置
constexpr inline uint32_t OTHER_WHEEL_INDEX(uint32_t v, uint32_t idx) { return (((v) >> (FIRST_WHEEL_BITS + (idx - 1) * OTHER_WHEEL_BITS)) & OTHER_WHEEL_MASK); }

using TimeWheelSlotInfo = std::array<uint32_t, WHEEL_COUNT>;

/**
 * 定时器任务，保存任务的关键数据
 */
struct TimeWheelTask : public TimeTask
{
    TimeWheelTask(TimerId a_timer_id, uint32_t a_interval, TimePoint a_target_time, const TimerCallback &cb)
        : TimeTask(a_timer_id, a_interval, a_target_time, cb) {}
    virtual ~TimeWheelTask() {
        m_slot_info.fill(0);
    }

    TimeWheelSlotInfo m_slot_info;
    uint32_t m_current_wheel_idx = 0;
};

/**
 * 时间轮
 *   每个时间轮有 SLOT_COUNT 个槽，每个槽可以存放多个定时器任务
 *  时间轮的槽是一个环形队列，每次更新时，将槽中的定时器任务移动到下一个槽中
 */
using TickResult = std::pair<std::list<TimeWheelTask*>, bool>;

template<int SLOT_COUNT> // 时间轮槽的数量
class CTimeWheel : public noncopyable
{
public:
    void AddTask(TimeWheelTask* ap_task)
    {
        m_slots[ap_task->m_slot_info[ap_task->m_current_wheel_idx]].push_back(ap_task);
    }
    TickResult Tick()
    {
        std::list<TimeWheelTask*>&& tasks = PopTask(m_current_slot);
        ++m_current_slot;
        if (m_current_slot == SLOT_COUNT)
        {
            m_current_slot = 0; // 回到第一个槽位
        }
        return std::make_pair(tasks, 0 == m_current_slot);
    }
    TickResult Degrade()
    {
        ++m_current_slot;
        if (m_current_slot == SLOT_COUNT)
        {
            m_current_slot = 0; // 回到第一个槽位
        }
        std::list<TimeWheelTask*>&& tasks = PopTask(m_current_slot);
        return std::make_pair(tasks, 0 == m_current_slot);
    }
    uint32_t GetCurrentSlot() const { return m_current_slot; }
protected:
    std::list<TimeWheelTask*> PopTask(uint32_t a_slot_idx)
    {
        std::list<TimeWheelTask*> tmp;
        if (a_slot_idx >= SLOT_COUNT)
        {
            return tmp;
        }
        tmp.swap(m_slots[a_slot_idx]);
        return tmp;
    }

private:
    std::array<std::list<TimeWheelTask*>, SLOT_COUNT> m_slots;
    uint32_t m_current_slot = 0;
};


class CTimeManagerImplWheel : public  CTimeManager::CTimeManagerImpl
{
public:
    CTimeManagerImplWheel()
        : m_loop_start_time(SystemClock::now()), m_last_tick_time(m_loop_start_time)
    {
        m_first_wheel = FirstWheelPtr(new CTimeWheel<FIRST_WHEEL_SIZE>());
        for (int i = 0; i < OTHER_WHEEL_COUNT; ++i)
        {
            m_other_wheels[i] = OtherWheelPtr(new CTimeWheel<OTHER_WHEEL_SIZE>());
        }
    }

    void Init() override
    {}
    /**
     * 添加定时器任务
     */
    TimerId AddTimer(uint32_t interval, const TimerCallback& cb) override
    {
        // 创建一个定时器任务
        TimeWheelTask* p_time_task = CreateTimeWheelTask(interval, cb);
        AddTimeWheelTask(p_time_task);
        LogInfo("AddTimer", p_time_task->m_timer_id, TimePointToString(p_time_task->m_target_time), interval);
        return p_time_task->m_timer_id;
    }
    
    /**
     * 删除定时器任务
     */
    void DelTimer(TimerId timerId) override
    {
        auto find_it = m_timer_record.find(timerId);
        if (find_it == m_timer_record.end())
        {
            return;
        }
        TimeWheelTask* p_time_task = find_it->second;
        p_time_task->m_is_canceled = true;
        // 不立即释放，当时间轮执行到该定时器任务时，再释放
    }

    /**
     * 创建TimeWheelTask对象，并预先计算好在时间轮中的位置
     */
    TimeWheelTask* CreateTimeWheelTask(uint32_t a_interval, const TimerCallback& a_cb)
    {
        // 分配一个定时器id
        TimerId timer_id = AllocTimerId();
        TimePoint target_time = SystemClock::now() + MilliSeconds(a_interval);
        MilliSeconds diff = DurationCastMilli(target_time - m_loop_start_time);
        // TODO 注意任务超过了一个时间轮的完整时间周期的情况，这里会截断时间。
        // 应该复用当前时间轮的位置前的slot，为下一轮周期创建任务，入参处需要判断任务时间不能超过一个时间轮周期
        uint32_t diff_step = (uint32_t)(diff.count() / ONE_STEP_MILLI); 
        TimeWheelTask* p_time_task = m_task_pool.Alloc(timer_id, a_interval, target_time, a_cb);
        
        for (int i = 0; i < WHEEL_COUNT; ++i)
        {
            p_time_task->m_slot_info[i] = 0 == i ? FIRST_WHEEL_INDEX(diff_step) : OTHER_WHEEL_INDEX(diff_step, i);
            LogDebug("slot_info", i, p_time_task->m_slot_info[i]);
        }
        LogDebug("m_slot_info", std::bitset<32>(diff_step));
        p_time_task->m_current_wheel_idx = 0; // 初始化在第一个轮中，后面会更新到正确的位置
        for (int i = WHEEL_COUNT - 1; i >= 0; --i)
        {
            if (p_time_task->m_slot_info[i] == 0)
            { // 不在这个轮子中
                continue;
            }
            uint32_t current_slot_idx = 0 == i ? FIRST_WHEEL_INDEX(m_time_wheel_tick) : OTHER_WHEEL_INDEX(m_time_wheel_tick, i);
            if (p_time_task->m_slot_info[i] == current_slot_idx)
            { // 这个轮子的位置和当前的位置相同, 说明这个任务在当前轮
                continue;
            }
            p_time_task->m_current_wheel_idx = i;
            break;
        }
        // LogDebug("m_current_wheel_idx", p_time_task->m_current_wheel_idx);
        // 将定时器任务添加到定时器记录中，用于后续删除
        m_timer_record.emplace(p_time_task->m_timer_id, p_time_task);
        return p_time_task;
    }
    

   /**
    * 添加定时器任务到时间轮中
    */
    void AddTimeWheelTask(TimeWheelTask* ap_task)
    {
        if (ap_task->m_is_canceled)
        {
            RemoveTimeWheelTask(ap_task);
            return;
        }
        // 获取该定时器处于哪个时间轮钟
        uint32_t current_wheel_idx = ap_task->m_current_wheel_idx;
        // 将定时器任务添加到对应时间轮钟中
        if (current_wheel_idx == 0)
        {
            m_first_wheel->AddTask(std::move(ap_task));
        }
        else
        {
            m_other_wheels[current_wheel_idx - 1]->AddTask(std::move(ap_task));
        }
    }
    
    /**
     * 删除定时器任务
     */
    void RemoveTimeWheelTask(TimeWheelTask* ap_task)
    {
        m_timer_record.erase(ap_task->m_timer_id); // TODO 对象池回收
        m_task_pool.Dealloc(ap_task);
    }

    void Update() override
    {
        TimePoint now = SystemClock::now();
        // 计算时间间隔，获得需要执行时间轮的步数
        auto diff_ms = DurationCastMilli(now - m_last_tick_time);
        if (diff_ms.count() < ONE_STEP_MILLI)
        {
            return; // 时间间隔小于步长，不需要更新时间轮
        }
        auto diff_step = (uint32_t)(diff_ms.count() / ONE_STEP_MILLI); // 注意处理完整周期的循环
        m_last_tick_time = now - MilliSeconds(diff_ms.count() % ONE_STEP_MILLI); // 计算出时间轮的误差

        for (uint32_t i = 0; i < diff_step; ++i)
        {
            TickOnce(now);
        }
    }
    
    /**
     * 执行一次时间轮的更新
     */
    void TickOnce(const TimePoint& now)
    {
        ++m_time_wheel_tick; // 时间轮前进一步
        // 处理第一个轮,返回触发的所有任务，以及第一个轮是否完成一次循环的结果
        TickResult result = m_first_wheel->Tick();
        // LogDebug("m_time_wheel_tick", std::bitset<32>(m_time_wheel_tick), m_time_wheel_tick, diff_milli.count(), diff_step, result.second);
        // LogDebug("tick check", std::bitset<32>(m_time_wheel_tick), std::bitset<32>(m_first_wheel->GetCurrentSlot()), result.second);
        for (TimeWheelTask* task : result.first)
        {
            ExecuteTask(task);
        }
        if (!result.second)
        {
            return;
        }
        // 如果第一个轮完成了一次循环，则处理下一个轮（更新新一轮任务）
        for (uint32_t j = 0; j < OTHER_WHEEL_COUNT; ++j)
        {
            // tick一次时间轮，走一个槽位，如果槽位中有任务，则返回出来。同时返回是否完成一次循环
            TickResult other_result = m_other_wheels[j]->Degrade();
            for (TimeWheelTask* p_task : other_result.first)
            {
                --p_task->m_current_wheel_idx; // 更新当前任务在时间轮中的位置
                AddTimeWheelTask(p_task); // 将任务放到对应的时间轮中
            }
            if (!other_result.second)
            {
                break; // 其他轮没有完成一次循环，则后面的轮不需要处理，直接退出
            }
        }
        if (0 == m_time_wheel_tick)
        { // 一个大周期完成，更新时间轮的起始时间 TODO 这里有误差，需要处理
            m_loop_start_time = now;
            m_last_tick_time = now;
        } 
    }

    /**
     * 执行定时器任务
     */
    void ExecuteTask(TimeWheelTask* ap_task)
    {
        if (ap_task->m_is_canceled)
        {
            return;
        }
        ap_task->m_cb();
        RemoveTimeWheelTask(ap_task); // 执行完任务后，回收任务
    }

private:

    using FirstWheelPtr = std::unique_ptr<CTimeWheel<FIRST_WHEEL_SIZE>>;
    using OtherWheelPtr = std::unique_ptr<CTimeWheel<OTHER_WHEEL_SIZE>>;

    FirstWheelPtr m_first_wheel; // 第一个轮，只有这个轮的任务才会被执行
    std::array<OtherWheelPtr, OTHER_WHEEL_COUNT> m_other_wheels;
    std::unordered_map<TimerId, TimeWheelTask*> m_timer_record;
    TimeWheelSlotInfo m_slot_info;
    TimerId m_timer_alloc_id = 0;
    TimePoint m_loop_start_time;
    TimePoint m_last_tick_time;
    uint32_t m_time_wheel_tick = 0;

    CObjectPool<TimeWheelTask> m_task_pool; // 对象池，定时器任务对象池
};