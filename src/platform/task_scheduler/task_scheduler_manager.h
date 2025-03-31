#pragma once

#include "base/types.h"
#include "src/task.h"
#include <map>
#include <functional>

class CTaskSchedulerManager: public CSingleton<CTaskSchedulerManager>
{
public:
    static CTaskSchedulerManager& Instance()
	{
		static CTaskSchedulerManager s_instance;
		return s_instance;
	}
	void Init();
	template<typename T, typename F, typename... Args>
	TaskId StartTask(T* obj, F&& f, Args&&... args)
	{
		TaskId taskId = AllocTaskId();
		auto task = [obj, f = std::forward<F>(f), ...args = std::forward<Args>(args)]() mutable -> CTask {
			co_await (obj->*f)(std::forward<Args>(args)...);
			co_return 1;
		}();
		m_tasks.emplace(taskId, std::move(task));  // 使用emplace和std::move
		return taskId;
	}
	
	void RemoveTask(TaskId taskId);
	void SuspendTask(TaskId taskId);
	void ResumeTask(TaskId taskId);

private:
    CTaskSchedulerManager();
    ~CTaskSchedulerManager();
	
    TaskId AllocTaskId()
    {
        return ++m_task_alloc_id;
    }

private:
    TaskId m_task_alloc_id = 0;
    friend class CSingleton<CTaskSchedulerManager>;

    std::map<TaskId, CTask> m_tasks;
};