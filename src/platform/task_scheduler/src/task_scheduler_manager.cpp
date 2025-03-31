#include "../task_scheduler_manager.h"
#include "logging.h"



CTaskSchedulerManager::CTaskSchedulerManager() 
{
    // Constructor implementation
}

CTaskSchedulerManager::~CTaskSchedulerManager()
{
}

void CTaskSchedulerManager::Init()
{
}



void CTaskSchedulerManager::RemoveTask(TaskId taskId)
{
    auto find_iter = m_tasks.find(taskId);
    if (find_iter != m_tasks.end())
    {
        m_tasks.erase(find_iter);
    }
}

void CTaskSchedulerManager::SuspendTask(TaskId taskId)
{
    // auto find_iter = m_tasks.find(taskId);
    // if (find_iter!= m_tasks.end())
    // {
    //     find_iter->second.GetHandle().;
    // }
}

void CTaskSchedulerManager::ResumeTask(TaskId taskId)
{
    auto find_iter = m_tasks.find(taskId);
    if (find_iter != m_tasks.end())
    {
        auto handle = find_iter->second.GetHandle();
        if (handle && !handle.done())  // 检查协程句柄是否有效且未结束
        {
            handle.resume();
        }
        else
        {
            LogDebug("Task {} is already done or invalid", taskId);
            RemoveTask(taskId); 
        }
    }
}

