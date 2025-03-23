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

// 通用日志输出函数
template<typename... Args>
void Log(LogLevel level, Args&&... args) {
    std::ostringstream oss;
    switch (level) {
        case LogLevel::DEBUG:
            oss << "debug ";
            break;
        case LogLevel::INFO:
            oss << "info ";
            break;
        case LogLevel::WARN:
            oss << "warn ";
            break;
        case LogLevel::ERROR:
            oss << "error ";
            break;
    }

    // 展开参数包并输出
    (oss << ... << std::forward<Args>(args)) << '\n';
    std::cout << oss.str();
}

// 定义日志宏
#define LogDebug(...) Log(LogLevel::DEBUG, __VA_ARGS__)
#define LogInfo(...) Log(LogLevel::INFO, __VA_ARGS__)
#define LogWarn(...) Log(LogLevel::WARN, __VA_ARGS__)
#define LogError(...) Log(LogLevel::ERROR, __VA_ARGS__)