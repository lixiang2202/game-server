#pragma once

#include "base/types.h"
#include "logging.h"

template<typename T>
class CObjectPool: public CNonCopyable
{
public:
    CObjectPool()
    {
        m_allocated_count = 0;
        m_free_list_head = nullptr;
        m_object_size = std::max(sizeof(T), sizeof(T*)); // 对象大小
    }
    ~CObjectPool()
    {
        if (m_allocated_count != 0)
        {
            LogError("Object pool not empty, allocated count: " , m_allocated_count);
        }
        while (m_free_list_head != nullptr)
        {
            T* p = m_free_list_head;
            m_free_list_head = *reinterpret_cast<T**>(m_free_list_head);
            free(p);
        }
    }

    template<typename... Args>
    T* Alloc(Args&&... args)
    {
        ++m_allocated_count;
        if (m_free_list_head == nullptr)
        {
            return new(malloc(m_object_size)) T(std::forward<Args>(args)...);
        }
        T* ptr = m_free_list_head;
        m_free_list_head = *reinterpret_cast<T**>(m_free_list_head);
        return new(ptr)T(std::forward<Args>(args)...);
    }

    void Dealloc(T* ptr)
    {
        --m_allocated_count;
        ptr->~T(); // 调用析构函数
        *reinterpret_cast<T**>(ptr) = m_free_list_head; // 将当前对象放入空闲链表中
        m_free_list_head = ptr; // 更新空闲链表头指针
    }


private:
    int m_allocated_count = 0; // 分配的对象数量
    size_t m_object_size = 0; // 对象大小
    T* m_free_list_head = nullptr;
};