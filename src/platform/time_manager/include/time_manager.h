/**
 * 高精度定时器
 *   多级时间轮实现, 支持毫秒级
 */
#pragma once

#include "base/types.h"

#include <functional>
using TimerCallback = std::function<void(void)>;

class CTimeManager : public noncopyable
{
public:
    static CTimeManager& Instance()
    {
        static CTimeManager s_instance;
        return s_instance;
    }
    TimerId AddTimer(uint32_t interval, const TimerCallback& cb);
    void DelTimer(TimerId timerId);
    void Update();

private:
    CTimeManager();
    ~CTimeManager();

    class CTimeManagerImpl; 
    CTimeManagerImpl* mp_impl = nullptr;
};

#define ADD_TIMER(interval, cb) CTimeManager::Instance().AddTimer(interval, cb)
#define DEL_TIMER(timerId) CTimeManager::Instance().DelTimer(timerId)