#include <iostream>
#include "util_common.h"
 
int _main_impl(int argc, char const *argv[])
{
    UNUSED(argc); 
    UNUSED(argv);  
    LogDebug("Hello, World!");
    return 0;
}
 
int main(int argc, char const *argv[])
{
    return _main_impl(argc, argv);
}