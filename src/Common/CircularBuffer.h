#pragma once

#include <vector>

namespace incolun {
namespace clothodb {

//T -type name
//C - creation function
//R - reset fucntion
template<class T, T(*Get)(), void(*Reset)(T&)>
class CircularBuffer
{
public:
    typedef T& reference;
    typedef const T& const_reference;

    CircularBuffer(size_t capacity) :m_capacity(capacity)
    {}

    T& operator[](size_t pos)
    {
        auto p = (first + pos) % buffer.size();
        return buffer[p];
    }

    bool IsEmpty() const { return m_empty; }

    size_t Size() const
    {
        if (IsEmpty()) return 0;
        return (m_tailIndex >= m_headIndex) ?
            (m_tailIndex - m_headIndex + 1) :
            (m_capacity - m_headIndex + m_tailIndex + 1);
    }

    void SetHeadIndex(size_t index)
    {
        if (index >= m_capacity) index = index % m_capacity;
        m_headIndex = index;
    }

    void SetTailIndex(size_t index)
    {
        if (index >= m_capacity) index = index % m_capacity;
        m_tailIndex = index;
    }

    void PushFront()
    {
        if (IsEmpty()) return;
        m_resetter(this->[0]);
        if (m_headIndex == m_tailIndex) m_empty = true;

        MoveIndexForward(m_headIndex);
    }

    void PushTail()
    {
        if (IsEmpty()) return;
        m_resetter(this->[Size()]);
        if (m_headIndex == m_tailIndex) m_empty = true;

        if (m_buffer.size() < m_capacity)
        {
            m_buffer.emplace_back(m_getter());
        }

        MoveIndexForward(m_tailIndex);
    }

private:
    void MoveIndexForward(size_t& index) const
    {
        ++index;
        if (index >= m_capacity) index = index % m_capacity;
    }

private:
    std::vector<T> m_buffer;
    Get m_getter;
    Reset m_reseter;
    size_t m_capacity;

    size_t m_headIndex = 0;
    size_t m_tailIndex = 0;
    bool m_empty = true;

    CircularBuffer() = delete;
    CircularBuffer(const CircularBuffer<T, T(*Get)(), void(*Reset)(T&)> &rhs) = delete;
    CircularBuffer(CircularBuffer<T, T(*Get)(), void(*Reset)(T&)>&& rhs) = delete;
};

}}