#include "platform/time_manager/time_manager.h"
#include "logging.h"

void func()
{
    LogDebug("func 100ms");
    ADD_TIMER(10000, []() {
        LogDebug("timer 10000");
    });
    ADD_TIMER(1234, []() {
        LogDebug("timer 1234");
    });
    ADD_TIMER(2345, []() {
        LogDebug("timer 2345");
    });
    // ADD_TIMER(100000, []() {
    //     LogDebug("timer 100000");
    // });
    // ADD_TIMER(99999, []() {
    //     LogDebug("timer 99999");
    // });
}
int main()
{
    CTimeManager::Instance().Init();
    ADD_TIMER(100, func);

    while(true)
    {
        CTimeManager::Instance().Update();
        // std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    return 0;
}