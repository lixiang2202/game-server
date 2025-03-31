/**
 * 定时器
 */
#pragma once

#include "base/types.h"

#include <functional>
using TimerCallback = std::function<void(void)>;
enum class TimerType
{
    USE_TIME_NORMAL = 0, // 普通定时器
    USE_TIME_WHEEL = 1, // 时间轮
};
class CTimeManager : public CSingleton<CTimeManager>
{
public:
    void Init();
    TimerId AddTimer(uint32_t interval, const TimerCallback& cb);
    void DelTimer(TimerId timerId);
    void Update();

private:
    CTimeManager();
    ~CTimeManager();
    friend class CSingleton<CTimeManager>;
    friend class CTimeManagerImplNormal; // 友元类，map实现类
    friend class CTimeManagerImplWheel; // 友元类，时间轮实现类

    class CTimeManagerImpl; 
    CTimeManagerImpl* mp_impl = nullptr;

    const TimerType m_timer_type;
};

#define ADD_TIMER(interval, cb) CTimeManager::Instance().AddTimer(interval, cb)
#define DEL_TIMER(timerId) CTimeManager::Instance().DelTimer(timerId)