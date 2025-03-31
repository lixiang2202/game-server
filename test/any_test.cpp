#include <iostream>
#include <chrono>
#include "base/time_helper.h"
#include "logging.h"

int main() 
{
    TimePoint now = SystemClock::now();
    TimePoint now2 = now + MilliSeconds(1000);
    LogDebug(TimePointToString(now2));
    return 0;
}