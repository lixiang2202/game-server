#include <iostream>
#include <chrono>
#include "base/time_helper.h"
#include "logging.h"

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
};

// 宏定义，用于创建闭包并添加到数组中
#define ADD_CLOSURE(closures, func, obj, ...) \
    closures.push_back(std::bind(func, obj, __VA_ARGS__))

int main() {
    Calculator calc;
    // 定义一个存储闭包的数组
    std::vector<std::function<void()>> closures;

    // 使用宏创建闭包并添加到数组中
    ADD_CLOSURE(closures, &Calculator::printSum, &calc, 3, 5);
    ADD_CLOSURE(closures, &Calculator::printProduct, &calc, 2, 4, 6);

    // 遍历数组并调用闭包
    for (const auto& closure : closures) {
        closure();
    }

    return 0;
}    