#include <iostream>
#include <chrono>
#include "base/time_helper.h"
#include "logging.h"
#include "base/closure.h"

#include <functional>
#include <iostream>
#include <functional>
#include <vector>
#include <iostream>
#include <functional>
#include <vector>

// 定义一个类
class Calculator {
public:
    // 类的成员函数，用于计算和并打印
    void printSum(int a, int b) {
        std::cout << "Sum: " << a + b << std::endl;
    }
    // 类的成员函数，用于计算乘积并打印
    void printProduct(int a, int b, int c) {
        std::cout << "Product: " << a * b * c << std::endl;
    }

    int sum(int a, int b) {
        return a + b; 
    }
};

// 宏定义，用于创建闭包并添加到数组中
// #define ADD_CLOSURE(closures, func, obj, ...) \
//     closures.push_back(std::bind(func, obj, __VA_ARGS__))
template<typename T, typename... Args>
void addClosure(std::vector<std::function<void()>>& closures, void(T::*func)(Args...), T* obj, Args... args) {
    closures.push_back(std::bind(func, obj, args...));
}
int main() {

    Calculator calc;
    CClosure closure(&calc, &Calculator::sum, 3, 5);
    auto ret = closure.Call();
    LogDebug("ret:", ret);
    // // 定义一个存储闭包的数组
    // std::vector<std::function<void()>> closures;

    // // 使用宏创建闭包并添加到数组中
    // addClosure(closures, &Calculator::printSum, &calc, 3, 5);
    // addClosure(closures, &Calculator::printProduct, &calc, 2, 4, 6);

    // // 遍历数组并调用闭包
    // for (const auto& closure : closures) {
    //     closure();
    // }

    return 0;
}    