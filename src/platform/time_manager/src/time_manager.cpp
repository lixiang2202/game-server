#include "time_manager.h"

#include "logging.h"
#include "time_wheel.h"

CTimeManager::CTimeManager()
{
    m_first_wheel = FirstWheelPtr(new CTimeWheel<FIRST_WHEEL_SIZE>());
    for (int i = 0; i < OTHER_WHEEL_COUNT; ++i)
    {
        m_other_wheels[i] = OtherWheelPtr(new CTimeWheel<OTHER_WHEEL_SIZE>());
    }
}

CTimeManager::~CTimeManager()
{
}

TimerId CTimeManager::AddTimer(uint32_t interval, const TimerCallback &cb)
{
    CTimeTask tt(interval, cb, this);
    return 0;
}

void CTimeManager::DelTimer(TimerId timerId)
{
}

void CTimeManager::Update()
{
}
