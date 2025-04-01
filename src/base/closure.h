#pragma once

#include "logging.h"
#include <functional>

template<typename T, typename F, typename... Args>
struct CClosure
{
    using ResultType = std::invoke_result_t<F, T*, Args...>;
    
    CClosure(T* obj, F&& f, Args&&... args) 
    {
        Assert(obj != nullptr, "obj is nullptr");
        m_closure = [obj, f = std::forward<F>(f), ...args = std::forward<Args>(args)]() -> ResultType {
                return (obj->*f)(args...);
            };
    }
    
    ResultType Call()
    {
        return m_closure();
    }
private:
    std::function<ResultType()> m_closure;
};

// 添加显式推导指引
template<typename T, typename F, typename... Args>
CClosure(T*, F&&, Args&&...) -> CClosure<T, F, Args...>;
