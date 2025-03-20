#include <iostream>
#include "util_common.h"

#include "test/coroutine20.h"
#include "platform/task_scheduler/include/task_scheduler_manager.h"
 
int _main_impl(int argc, char const *argv[])
{
    UNUSED(argc); 
    UNUSED(argv);  
    LogDebug("Hello, World!");
    return 0;
}

int _main_test_task_scheduler_manager()
{
    CTaskSchedulerManager task_scheduler_manager;
    // task_scheduler_manager.Start();
    // task_scheduler_manager.Stop();
    return 0;
}
 
int main(int argc, char const *argv[])
{
    // return _main_impl(argc, argv);
    return _main_test_task_scheduler_manager();
}