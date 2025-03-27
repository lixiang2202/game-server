#include "time_manager.h"

#include "logging.h"

#include <array>
#include <memory>
#include <unordered_map>
#include <chrono>

// 第一个轮占的位数
constexpr int8_t FIRST_WHEEL_BITS = 8;
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

constexpr uint32_t TIME_WHEEL_STEP = 1; // 步长，第一个时间轮的最小单位 TODO 可作为配置
using TimeWheelSlotInfo = std::array<uint32_t, WHEEL_COUNT>;
using MilliSeconds = std::chrono::milliseconds;
using TimePoint = std::chrono::steady_clock::time_point;

/**
 * 定时器任务，保存任务的关键数据
 */
struct TimeTask : public noncopyable
{
    TimeTask(TimerId a_timer_id, TimePoint a_target_time, const TimerCallback &cb)
        : m_timer_id(a_timer_id), m_target_time(a_target_time), m_cb(cb) {}
    ~TimeTask()
    {
        m_cb = nullptr;
    }

    TimerId m_timer_id = 0;
    TimePoint m_target_time;
    TimerCallback m_cb;
    TimeWheelSlotInfo m_slot_info;
    uint32_t m_current_wheel_idx = 0;
    bool m_is_canceled = false;
};

/**
 * 时间轮
 *   每个时间轮有 SLOT_COUNT 个槽，每个槽可以存放多个定时器任务
 *  时间轮的槽是一个环形队列，每次更新时，将槽中的定时器任务移动到下一个槽中
 */
template<int SLOT_COUNT> // 时间轮槽的数量
class CTimeWheel : public noncopyable
{
public:
    using TickResult = std::pair<std::list<TimeTask*>, bool>;
    void AddTask(TimeTask* ap_task)
    {
        m_slots[ap_task->m_slot_info[ap_task->m_current_wheel_idx]].push_back(ap_task);
    }
    TickResult Tick()
    {
        ++m_current_slot;
        if (m_current_slot >= SLOT_COUNT)
        {
            m_current_slot = 0;
        }
        TickResult result = std::make_pair(PopTask(m_current_slot), 0 == m_current_slot);
        return result;
    }
    uint32_t GetCurrentSlot() const { return m_current_slot; }
protected:
    std::list<TimeTask*> PopTask(uint32_t a_slot_idx)
    {
        std::list<TimeTask*> tmp;
        if (a_slot_idx >= SLOT_COUNT)
        {
            return tmp;
        }
        tmp.swap(m_slots[a_slot_idx]);
        return tmp;
    }

private:
    std::array<std::list<TimeTask*>, SLOT_COUNT> m_slots;
    uint32_t m_current_slot = 0;
};

class CTimeManager::CTimeManagerImpl : public noncopyable
{
private:
    friend class CTimeManager;
    CTimeManagerImpl()
        : m_loop_start_time(std::chrono::steady_clock::now()), m_last_tick_time(m_loop_start_time)
    {
        m_first_wheel = FirstWheelPtr(new CTimeWheel<FIRST_WHEEL_SIZE>());
        for (int i = 0; i < OTHER_WHEEL_COUNT; ++i)
        {
            m_other_wheels[i] = OtherWheelPtr(new CTimeWheel<OTHER_WHEEL_SIZE>());
        }
    }
    TimerId AddTimer(uint32_t interval, const TimerCallback& cb)
    {
        // 创建一个定时器任务
        TimeTask* p_time_task = CreateTimeTask(interval, cb);
        // 获取该定时器处于哪个时间轮钟
        uint32_t current_wheel_idx = p_time_task->m_current_wheel_idx;
        // 将定时器任务添加到对应时间轮钟中
        if (current_wheel_idx == 0)
        {
            m_first_wheel->AddTask(std::move(p_time_task));
        }
        else
        {
            m_other_wheels[current_wheel_idx - 1]->AddTask(std::move(p_time_task));
        }
        // 将定时器任务添加到定时器记录中，用于后续删除
        m_timer_record.emplace(p_time_task->m_timer_id, p_time_task);
        return p_time_task->m_timer_id;
    }

    TimeTask* CreateTimeTask(uint32_t interval, const TimerCallback& cb)
    {
        // 分配一个定时器id
        TimerId timer_id = AllocTimerId();
        TimePoint target_time = std::chrono::steady_clock::now() + std::chrono::milliseconds(interval);
        MilliSeconds diff = std::chrono::duration_cast<MilliSeconds>(target_time - m_loop_start_time);
        // TODO 注意任务超过了一个时间轮的完整时间周期的情况，这里会截断时间。
        // 应该复用当前时间轮的位置前的slot，为下一轮周期创建任务，入参处需要判断任务时间不能超过一个时间轮周期
        uint32_t diff_step = (uint32_t)(diff.count() / TIME_WHEEL_STEP); 
        // (TODO TimeManager 对象池)
        TimeTask* p_time_task = new TimeTask(timer_id, target_time, cb);
        
        for (int i = 0; i < WHEEL_COUNT; ++i)
        {
            p_time_task->m_slot_info[i] = 0 == i ? FIRST_WHEEL_INDEX(diff_step) : OTHER_WHEEL_INDEX(diff_step, i);
            LogDebug("slot_info", i, p_time_task->m_slot_info[i]);
        }
        const TimeWheelSlotInfo& current_slot_info = m_slot_info;
        p_time_task->m_current_wheel_idx = 0; // 初始化在第一个轮中，后面会更新到正确的位置
        for (int i = WHEEL_COUNT - 1; i >= 0; --i)
        {
            if (p_time_task->m_slot_info[i] == 0)
            { // 不在这个轮子中
                continue;
            }
            if (p_time_task->m_slot_info[i] == current_slot_info[i])
            { // 这个轮子的位置和当前的位置相同, 说明这个任务在当前轮
                continue;
            }
            p_time_task->m_current_wheel_idx = i;
            break;
        }
        LogDebug("m_current_wheel_idx", p_time_task->m_current_wheel_idx);
        return p_time_task;
    }
    void DelTimer(TimerId timerId)
    {
        auto find_it = m_timer_record.find(timerId);
        if (find_it == m_timer_record.end())
        {
            return;
        }
        TimeTask* p_time_task = find_it->second;
        p_time_task->m_is_canceled = true;
        // 不立即释放，当时间轮执行到该定时器任务时，再释放
    }

    void Update()
    {
        TimePoint now = std::chrono::steady_clock::now();
        // 计算时间间隔
        MilliSeconds diff = std::chrono::duration_cast<MilliSeconds>(now - m_last_tick_time);
        uint32_t diff_step = (uint32_t)(diff.count() / TIME_WHEEL_STEP); // 注意处理完整周期的循环
        for (uint32_t i = 0; i < diff_step; ++i)
        {
            ++m_time_wheel_tick; // 时间轮前进一步
            // 处理第一个轮,返回触发的所有任务，以及第一个轮是否完成一次循环的结果
            m_first_wheel->Tick();
            // 如果第一个轮完成了一次循环，则处理下一个轮（更新新一轮任务）
            for (int j = 0; j < OTHER_WHEEL_COUNT; ++j)
            {
                // tick一次时间轮，走一个槽位，如果槽位中有任务，则返回出来。同时返回是否完成一次循环
                m_other_wheels[j]->Tick();
                // 如果没有完成一次循环，则后面的轮不需要处理，直接退出
            }
            if (0 == m_time_wheel_tick)
            { // 一个大周期完成，更新时间轮的起始时间 TODO 这里有误差，需要处理
                m_loop_start_time = now;
                m_last_tick_time = now;
            }
        }
    }

    TimerId AllocTimerId() { return ++m_timer_alloc_id; }
private:

    using FirstWheelPtr = std::unique_ptr<CTimeWheel<FIRST_WHEEL_SIZE>>;
    using OtherWheelPtr = std::unique_ptr<CTimeWheel<OTHER_WHEEL_SIZE>>;

    FirstWheelPtr m_first_wheel; // 第一个轮，只有这个轮的任务才会被执行
    std::array<OtherWheelPtr, OTHER_WHEEL_COUNT> m_other_wheels;
    std::unordered_map<TimerId, TimeTask*> m_timer_record;
    TimeWheelSlotInfo m_slot_info;
    TimerId m_timer_alloc_id = 0;
    TimePoint m_loop_start_time;
    TimePoint m_last_tick_time;
    uint32_t m_time_wheel_tick = 1;
};

CTimeManager::CTimeManager(): mp_impl(new CTimeManagerImpl())
{
}

CTimeManager::~CTimeManager()
{
}

TimerId CTimeManager::AddTimer(uint32_t interval, const TimerCallback &cb)
{
    return mp_impl->AddTimer(interval, cb);
}

void CTimeManager::DelTimer(TimerId timerId)
{
    mp_impl->DelTimer(timerId);
}

void CTimeManager::Update()
{
    mp_impl->Update();
}

