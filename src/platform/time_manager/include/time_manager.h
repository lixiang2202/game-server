/**
 * 定时器
 */
#pragma once

#include "base/types.h"

#include <functional>
#include <map>
using TimerCallback = std::function<void(void)>;
enum class TimerType
{
    USE_TIME_NORMAL = 0, // 普通定时器
    USE_TIME_HEAP = 1, // 堆定时器(小顶堆)
    USE_TIME_WHEEL = 2, // 时间轮
};
class CTimeManager : public noncopyable
{
public:
    static CTimeManager& Instance()
    {
        static CTimeManager s_instance;
        return s_instance;
    }
    void Init();
    TimerId AddTimer(uint32_t interval, const TimerCallback& cb);
    void DelTimer(TimerId timerId);
    void Update();

private:
    CTimeManager();
    ~CTimeManager();
    friend class CTimeManagerImplNormal; // 友元类，map实现类
    friend class CTimeManagerImplWheel; // 友元类，时间轮实现类

    class CTimeManagerImpl; 
    CTimeManagerImpl* mp_impl = nullptr;

    TimerType m_timer_type = TimerType::USE_TIME_NORMAL;
};

#define ADD_TIMER(interval, cb) CTimeManager::Instance().AddTimer(interval, cb)
#define DEL_TIMER(timerId) CTimeManager::Instance().DelTimer(timerId)