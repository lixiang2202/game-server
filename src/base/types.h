#pragma once

namespace util
{
    class CNonCopyable
    {
    public:
        CNonCopyable() = default;
        ~CNonCopyable() = default;
        CNonCopyable(const CNonCopyable&) = delete;
        CNonCopyable& operator=(const CNonCopyable&) = delete;
    };
}
using noncopyable = util::CNonCopyable;

#include <cstdint>
using TimerId = uint64_t;
