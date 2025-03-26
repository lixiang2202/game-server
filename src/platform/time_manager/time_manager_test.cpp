#include "time_manager_test.h"
#include "include/time_manager.h"
#include "logging.h"

void func()
{
    LogDebug("func");
}
int _time_manager_test()
{
    ADD_TIMER(1000, []() {
        LogDebug("timer 1s");
    });
    ADD_TIMER(10000, []() {
        LogDebug("timer 1s");
    });
    ADD_TIMER(100, []() {
        LogDebug("timer 1s");
    });
    ADD_TIMER(10, func);
    return 0;
}