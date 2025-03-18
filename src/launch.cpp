#include <iostream>

#define UNUSED(x) (void)(x)

int _main_impl(int argc, char const *argv[])
{
    UNUSED(argc); 
    UNUSED(argv);  
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
 
int main(int argc, char const *argv[])
{
    return _main_impl(argc, argv);
}