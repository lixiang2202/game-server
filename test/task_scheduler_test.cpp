#pragma once

#include "logging.h"
#include "platform/task_scheduler/task_scheduler_manager.h"
#include "platform/time_manager/time_manager.h"

class TestTask
{
public:
    CTask TaskFunc(int a, int b)
    {
        LogDebug("TaskFunc1", a, b);
        co_yield 1;
        LogDebug("TaskFunc2", a, b);
        co_yield 0;
    }
};

int main()
{
    LogDebug("Task Scheduler Test Start");
    CTaskSchedulerManager::Instance().Init();
    TestTask testTask;

    CTimeManager::Instance().Init();
    ADD_TIMER(1000, [&]() { 
        TaskId id = CTaskSchedulerManager::Instance().StartTask<TestTask>(&testTask, &TestTask::TaskFunc, 1, 2);
        ADD_TIMER(1000, [id]() {
            CTaskSchedulerManager::Instance().ResumeTask(id);
        });
    });
    while (true)
    {
        CTimeManager::Instance().Update();
    }
    return 0;
}