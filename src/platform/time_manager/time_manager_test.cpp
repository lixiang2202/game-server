#include "time_manager_test.h"
#include "time_manager.h"
#include <chrono>
#include <thread>
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
int _time_manager_test()
{
    // ADD_TIMER(100, []() {
    //     LogDebug("timer 100ms");
    // });
    // ADD_TIMER(1000, []() {
    //     LogDebug("timer 1000ms");
    // });
    // ADD_TIMER(10000, []() {
    //     LogDebug("timer 10000");
    // });
    // ADD_TIMER(100000, []() {
    //     LogDebug("timer 100000");
    // });
    // ADD_TIMER(200000, []() {
    //     LogDebug("timer 200000");
    // });

    CTimeManager::Instance().Init();
    ADD_TIMER(100, func);

    while(true)
    {
        CTimeManager::Instance().Update();
        // std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    // auto m_last_tick_time = std::chrono::system_clock::now();
    // while (true)
    // {
    //     auto now = std::chrono::system_clock::now();
    //     // 计算时间间隔
    //     auto diff = std::chrono::duration_cast<std::chrono::microseconds>(now - m_last_tick_time);
    //     if (diff.count() < 1000)
    //     {
    //         continue;
    //     }
    //     m_last_tick_time = now;
    //     std::cerr << "diff: " << diff.count()/1000 << std::endl;
    // }

    return 0;
}