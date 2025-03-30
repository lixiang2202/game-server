#include "time_manager.h"
#include "logging.h"
#include "time_manager_impl_normal.h"
#include "time_manager_impl_wheel.h"

CTimeManager::CTimeManager() //: mp_impl(new CTimeManagerImpl())
{

}

CTimeManager::~CTimeManager()
{
}

void CTimeManager::Init()
{
    switch (m_timer_type)
    {
    case TimerType::USE_TIME_NORMAL:
        mp_impl = new CTimeManagerImplNormal();
        break;
    case TimerType::USE_TIME_WHEEL:
        mp_impl = new CTimeManagerImplWheel();
        break;
    default:
        break;
    }
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

