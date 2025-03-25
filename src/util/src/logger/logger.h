#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

// 定义日志级别枚举
enum class LogLevel {
    DEBUG,
    INFO,
    WARN,
    ERROR
};

// 定义日志级别对应的前缀
constexpr const char* logLevelPrefixes[] = {
    "\033[36m【DEBUG】", // DEBUG: 青色
    "\033[32m【INFO】",  // INFO: 绿色
    "\033[33m【WARN】", // WARN: 黄色
    "\033[31m【ERROR】" // ERROR: 红色
};

constexpr const char* resetColor = "\033[0m";

// 通用日志输出函数
template<typename... Args>
void Log(LogLevel level, Args&&... args) {
    std::ostringstream oss;
    // 使用数组来获取日志级别前缀
    oss << logLevelPrefixes[static_cast<size_t>(level)];

    // 定义一个 lambda 函数用于将参数添加到流中，并在每个参数后添加空格
    auto append_args = [&oss](auto&& arg) {
        oss << std::forward<decltype(arg)>(arg) << ' ';
    };
    // 使用折叠表达式调用 lambda 函数处理每个参数
    (append_args(std::forward<Args>(args)), ...);
    // 添加重置颜色的转义序列
    oss << resetColor << '\n';

    std::cout << oss.str();
}

// 定义日志宏
#define LogDebug(...) Log(LogLevel::DEBUG, __VA_ARGS__)
#define LogInfo(...) Log(LogLevel::INFO, __VA_ARGS__)
#define LogWarn(...) Log(LogLevel::WARN, __VA_ARGS__)
#define LogError(...) Log(LogLevel::ERROR, __VA_ARGS__)