#pragma once
#include "base/time_helper.h"

constexpr uint32_t ONE_STEP_MILLI = 1; // 步长，第一个时间轮的最小单位，单位毫秒 TODO 可作为配置
constexpr uint32_t ONE_STEP_MICRO = 1000 * ONE_STEP_MILLI; // 步长，单位微秒

class CTimeManager::CTimeManagerImpl : public noncopyable
{
public:
    virtual void Init() = 0;
    virtual TimerId AddTimer(uint32_t interval, const TimerCallback& cb) = 0;
    virtual void DelTimer(TimerId timerId) = 0;
    virtual void Update() = 0;

    TimerId AllocTimerId() { return ++m_timer_alloc_id; }
private:
    TimerId m_timer_alloc_id = 0;
};

/**
 * 定时器任务，保存任务的关键数据
 */
struct TimeTask : public noncopyable
{
    TimeTask(TimerId a_timer_id, uint32_t a_interval, TimePoint a_target_time, const TimerCallback &cb)
        : m_timer_id(a_timer_id), m_interval(a_interval), m_target_time(a_target_time), m_cb(cb) {}
    ~TimeTask() {
        m_cb = nullptr;
    }

    TimerId m_timer_id = 0;
    TimerCallback m_cb;
    TimePoint m_target_time;
    uint32_t m_interval;
    bool m_is_canceled = false;
};