/**
 * 高精度定时器
 *   多级时间轮实现, 支持毫秒级
 */
#pragma once

#include "base/types.h"

#include <functional>
#include <array>
#include <memory>


// 第一个轮占的位数
constexpr int8_t FIRST_WHEEL_BITS = 8;
// 第一个轮的长度
constexpr int32_t FIRST_WHEEL_SIZE = 1 << FIRST_WHEEL_BITS; // 256
// 其他轮占的位数
constexpr int8_t OTHER_WHEEL_BITS = 6;
// 其他轮的长度
constexpr int32_t OTHER_WHEEL_SIZE = 1 << OTHER_WHEEL_BITS; // 64
// 其他轮数量
constexpr int8_t OTHER_WHEEL_COUNT = (32 - FIRST_WHEEL_BITS) / OTHER_WHEEL_BITS; // 4;
// 总共的时间轮数量
constexpr int8_t WHEEL_COUNT = OTHER_WHEEL_COUNT + 1; // 5

// 第一个轮掩码
constexpr uint32_t FIRST_WHEEL_MASK = FIRST_WHEEL_SIZE - 1; // 0b11111111
// 其他轮掩码
constexpr uint32_t OTHER_WHEEL_MASK = OTHER_WHEEL_SIZE - 1; // 0b111111

// 第一个轮的当前位置
constexpr inline uint32_t FIRST_WHEEL_INDEX(uint32_t v) { return v & FIRST_WHEEL_MASK; }
// 其他轮的当前位置
constexpr inline uint32_t OTHER_WHEEL_INDEX(uint32_t v, uint32_t idx) { return (((v) >> (FIRST_WHEEL_BITS + (idx - 1) * OTHER_WHEEL_BITS)) & OTHER_WHEEL_MASK); }


using TimerCallback = std::function<void(void)>;
using TimerId = uint32_t;
using TimeWheelSlotInfo = std::array<uint32_t, WHEEL_COUNT>;

class CTimeTask;
template<int> class CTimeWheel;

class CTimeManager : public noncopyable
{
public:
    CTimeManager();
    ~CTimeManager();
    TimerId AddTimer(uint32_t interval, const TimerCallback& cb);
    void DelTimer(TimerId timerId);

    void Update();

private:

    using FirstWheelPtr = std::unique_ptr<CTimeWheel<FIRST_WHEEL_SIZE>>;
    using OtherWheelPtr = std::unique_ptr<CTimeWheel<OTHER_WHEEL_SIZE>>;

    FirstWheelPtr m_first_wheel;
    std::array<OtherWheelPtr, OTHER_WHEEL_COUNT> m_other_wheels;
};
