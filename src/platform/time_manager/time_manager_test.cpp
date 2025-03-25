#include "time_manager_test.h"
#include "include/time_manager.h"
#include "logging.h"

int _time_manager_test()
{
    CTimeManager tm;
    tm.AddTimer(1000, []() {
        LogDebug("timer 1s");
    });
    return 0;
}