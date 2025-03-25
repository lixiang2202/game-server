#include "time_task.h"
#include "time_manager.h"
#include "logging.h"

CTimeTask::CTimeTask(uint32_t interval, const TimerCallback &cb, CTimeManager *ap_time_manager)
    : m_interval(interval)
    , m_cb(cb)
    , mp_time_manager(ap_time_manager)
{
    for (int i = 0; i < WHEEL_COUNT; ++i)
    {
        m_slot_info[i] = 0 == i ? FIRST_WHEEL_INDEX(interval) : OTHER_WHEEL_INDEX(interval, i);
        LogDebug("slot_info", i, m_slot_info[i]);
    }
}