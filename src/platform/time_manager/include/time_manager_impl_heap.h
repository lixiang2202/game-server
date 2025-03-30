#pragma once

#include "time_manager_impl.h"

class CTimeManagerImplHeap : public CTimeManager::CTimeManagerImpl
{

public:
    virtual void Init() override
    {}
    virtual TimerId AddTimer(uint32_t interval, const TimerCallback &cb) override
    {
        return 0;
    }
    virtual void DelTimer(TimerId timerId) override
    {}
    virtual void Update() override
    {}
};