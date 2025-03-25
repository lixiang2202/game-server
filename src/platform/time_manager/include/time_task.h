#pragma once

#include <chrono>
#include "base/types.h"
#include "time_manager.h"

using MilliSeconds = std::chrono::milliseconds;
using TimePoint = std::chrono::steady_clock::time_point;

class CTimeManager;
class CTimeTask : public noncopyable
{
public:
    CTimeTask(uint32_t interval, const TimerCallback& cb, CTimeManager* ap_time_manager);
    ~CTimeTask()
    {
        m_cb = nullptr;
    }
private:
    uint32_t m_interval;
    TimerCallback m_cb;
    CTimeManager* mp_time_manager; 
    TimeWheelSlotInfo m_slot_info;
};