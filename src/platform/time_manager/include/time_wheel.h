/**
 * 实现时间轮
 */
#pragma once

#include <list>
#include <memory>

#include "time_task.h"
#include "time_manager.h"

class CTimeTask;
using TimerTaskList = std::list<std::shared_ptr<CTimeTask>>;

template<int SLOT_COUNT> // 时间轮槽的数量
class CTimeWheel : public noncopyable
{
public:

private:
    std::array<TimerTaskList, SLOT_COUNT> m_slots;
};