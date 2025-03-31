#pragma once

class CNonCopyable
{
public:
    CNonCopyable() = default;
    ~CNonCopyable() = default;
    CNonCopyable(const CNonCopyable&) = delete;
    CNonCopyable& operator=(const CNonCopyable&) = delete;
};

template <typename T>
class CSingleton : public CNonCopyable
{
public:
    static T& Instance()
    {
        static T s_instance;
        return s_instance;
    }
};

#include <cstdint>
using TimerId = uint64_t;
using TaskId = uint64_t;
