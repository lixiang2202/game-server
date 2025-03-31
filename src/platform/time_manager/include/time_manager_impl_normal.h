#pragma once

#include "time_manager_impl.h"
#include "base/object_pool.h"

#include <map>


class CTimeManagerImplNormal : public CTimeManager::CTimeManagerImpl
{
public:
    CTimeManagerImplNormal()
        : m_last_tick_time(GetCurrentTimeMs()), m_total_cycle(0)
    {}
    virtual void Init() override
    {}
    virtual TimerId AddTimer(uint32_t a_interval, const TimerCallback &a_cb) override
    {
        TimerId timer_id = AllocTimerId();
        uint64_t diff_ms = GetCurrentTimeMs() - m_last_tick_time + a_interval; // 计算当前周期数
        uint64_t diff_cycle = diff_ms / ONE_STEP_MILLI; // 计算当前周期数
        if (diff_ms % ONE_STEP_MILLI > 0) // 如果当前周期数不是整数，则需要加1
        {
            ++diff_cycle;
        }
        uint64_t current_cycle = m_total_cycle + diff_cycle; // 计算当前周期数
        TimePoint target_time = SystemClock::now() + MilliSeconds(a_interval);
        TimeTask* p_time_task = m_task_pool.Alloc(timer_id, a_interval, target_time, a_cb);
        LogDebug("AddTimer", timer_id, current_cycle, TimePointToString(target_time), a_interval);
        m_timer_map.insert(std::make_pair(current_cycle, p_time_task)); // 插入定时器任务
        m_timer_record.insert(std::make_pair(timer_id, p_time_task)); // 插入定时器任务记录
        return 0;
    }
    virtual void DelTimer(TimerId timerId) override
    {
        auto it = m_timer_record.find(timerId); // 查找定时器任务记录
        if (it != m_timer_record.end()) // 如果找到了定时器任务记录
        {
            TimeTask* p_time_task = it->second; // 获取定时器任务
            p_time_task->m_is_canceled = true; // 标记为取消
        }
    }
    virtual void Update() override
    {
        uint64_t current_ms = GetCurrentTimeMs(); // 获取当前时间
        uint64_t diff_cycle = (current_ms - m_last_tick_time) / ONE_STEP_MILLI; // 计算当前周期数
        if (diff_cycle <= 0) // 如果没有周期变化，则不需要更新
        {
            return;
        }
        m_last_tick_time = current_ms; // 更新时间
        m_total_cycle += diff_cycle; // 更新总周期数

        for (auto iter = m_timer_map.begin(); iter != m_timer_map.end(); ) // 遍历定时器任务
        {
            if (iter->first > m_total_cycle) // 如果定时器任务的周期数大于总周期数，则不需要更新
            {
                break; // 退出循环
            }
            TimeTask* p_time_task = iter->second; // 获取定时器任务
            if (!p_time_task->m_is_canceled) // 如果定时器任务被取消了，则不需要更新
            {
                p_time_task->m_cb(); // 执行定时器任务的回调函数
            }
            m_timer_map.erase(iter++); // 删除定时器任务
            LogDebug("ExecuteTask", p_time_task->m_timer_id, TimePointToString(p_time_task->m_target_time), p_time_task->m_interval);
            m_task_pool.Dealloc(p_time_task); // 释放定时器任务
        }
    }

private:
    CObjectPool<TimeTask> m_task_pool; // 对象池，定时器任务对象池
    
    std::multimap<uint64_t, TimeTask*> m_timer_map; 
    std::unordered_map<TimerId, TimeTask*> m_timer_record; // 定时器任务记录
    const TimerType m_timer_type = TimerType::USE_TIME_WHEEL; // 定时器类型

    uint64_t m_total_cycle = 0; // 总周期数（ONE_STEP_MILLI一周期）
    uint64_t m_last_tick_time = 0; // 上次更新时间
};