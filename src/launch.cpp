#include <iostream>
#include <format>
#include "util_common.h"

int _main_impl(int argc, char const *argv[])
{
    UNUSED(argc); 
    UNUSED(argv);  
    LogDebug("Hello, World!");
    return 0;
}


void ShowGameServerInfo()
{
    std::cerr << "\033[32m" 
        << std::format("game-server Starting... \r\n  - version: {} \r\n  - build timestamp: {}"
            , VERSION_FULL
            , BUILD_TIMESTAMP) 
        << "\033[m" 
        << std::endl;
}
 
void OnTerminate()
{
    std::cerr << "\033[32m" << "game-server Terminating..." << "\033[m" << std::endl;
}

int main(int argc, char const *argv[])
{
    atexit(OnTerminate);
    ShowGameServerInfo();
    return _main_impl(argc, argv);
}