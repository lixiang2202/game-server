#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <chrono>

using MilliSeconds = std::chrono::milliseconds;
using MicroSeconds = std::chrono::microseconds;
using Seconds = std::chrono::seconds;
using TimePoint = std::chrono::system_clock::time_point;
using SystemClock = std::chrono::system_clock;

#define DurationCastMilli(v) std::chrono::duration_cast<std::chrono::milliseconds>((v))
#define DurationCastMicro(v) std::chrono::duration_cast<std::chrono::microseconds>((v))
#define DurationCastSecond(v) std::chrono::duration_cast<std::chrono::seconds>((v))

// std::chrono::steady_clock::time_point转为字符串，精度到毫秒
std::string TimePointToString(std::chrono::system_clock::time_point a_tp) 
{
    auto time_sec = std::chrono::system_clock::to_time_t(a_tp);
    auto time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        a_tp.time_since_epoch()) % 1000;
    std::tm tm;
    localtime_s(&tm, &time_sec);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S")
        << "." << std::setfill('0') << std::setw(3) << time_ms.count();
    return oss.str();
}

// 获取当前时间
time_t GetCurrentTime() { return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()); }

// 获取当前时间（毫秒级）
uint64_t GetCurrentTimeMs() { return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(); }