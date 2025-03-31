#pragma once

#include <coroutine>
#include <utility>
#include <functional>
#include "logging.h"

// 前向声明
struct CTaskAwaiter;
class CTask;

class CTask
{
public:
    struct promise_type
    {
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        CTask get_return_object();
        // void return_void() {}
        void return_value(int value) {}
        std::suspend_always yield_value(int value) { return {};}

        void unhandled_exception() {}
        CTaskAwaiter await_transform(CTask&& task);
    };

    using HandleType = std::coroutine_handle<promise_type>;
    explicit CTask(HandleType handle) : m_handle(handle) {}
    CTask(CTask&& other) noexcept : m_handle(std::exchange(other.m_handle, {})) {}
    CTask(const CTask&) = delete;
    CTask& operator=(const CTask&) = delete;
    ~CTask() 
    {
        if (m_handle) m_handle.destroy(); 
    }
    HandleType GetHandle() const { return m_handle; }

private:
    HandleType m_handle;
};

struct CTaskAwaiter
{
    explicit CTaskAwaiter(CTask&& a_task) noexcept : m_task(std::move(a_task)) {}
    CTaskAwaiter(CTaskAwaiter&& other) noexcept : m_task(std::move(other.m_task)) {}
    CTaskAwaiter(const CTaskAwaiter&) = delete;
    bool await_ready() const noexcept { return false; }
    void await_suspend(std::coroutine_handle<> awaiting_coroutine) const noexcept;
    void await_resume() const noexcept 
    {
    }

private:
    CTask m_task;
};

// 在类外定义成员函数
inline CTask CTask::promise_type::get_return_object()
{
    return CTask{std::coroutine_handle<promise_type>::from_promise(*this)};
}

inline CTaskAwaiter CTask::promise_type::await_transform(CTask&& task)
{
    return CTaskAwaiter(std::move(task));
}

inline void CTaskAwaiter::await_suspend(std::coroutine_handle<> awaiting_coroutine) const noexcept
{
    // 实现代码
}