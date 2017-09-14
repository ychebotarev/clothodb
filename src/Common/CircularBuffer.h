#pragma once

#include <vector>

namespace incolun {
namespace clothodb {

//T -type name
//C - creation function
//R - reset fucntion
template<class T>
class CircularBuffer
{
public:
    template<typename T> using Getter = T(*)();
    template<typename T> using Resetter = void(*)(T&);

    CircularBuffer(size_t capacity, Getter<T> getter, Resetter<T> resetter) : 
        m_capacity(capacity),
        m_getter(getter),
        m_resetter(resetter)
    {}

    T& operator[](size_t pos)
    {
        return At(pos);
    }

    T& At(size_t pos)
    {
        if(pos >= m_capacity)
        {
            throw std::out_of_range("index our of range");
        }
        if (m_headIndex >= m_tailIndex &&
            (pos < m_tailIndex || pos > m_headIndex))
        {
            throw std::out_of_range("index our of range");
        }

        auto p = (first + pos) % buffer.size();
        return buffer[p];
    }

    bool IsEmpty() const { return m_empty; }

    size_t Size() const
    {
        if (IsEmpty()) return 0;
        return (m_headIndex >= m_tailIndex) ?
            (m_headIndex - m_tailIndex + 1) :
            (m_capacity - m_tailIndex + m_headIndex + 1);
    }

    void SetHeadIndex(size_t index)
    {
        m_empty = false;
        if (index >= m_capacity) index = index % m_capacity;
        m_headIndex = index;
    }

    void SetTailIndex(size_t index)
    {
        m_empty = false;
        if (index >= m_capacity) index = index % m_capacity;
        m_tailIndex = index;
    }

    T& Head()
    {
        if (IsEmpty())
        {
            throw std::out_of_range("buffer is empty");
        }
        return m_buffer[m_headIndex];
    }

    T& Tail()
    {
        if (IsEmpty())
        {
            throw std::out_of_range("buffer is empty");
        }
        return m_buffer[m_tailIndex];
    }

    void MoveHeadForward()
    {
        if (IsEmpty())
        {
            m_empty = false;
            m_headIndex = 0;
            m_tailIndex = 0;

            if (m_buffer.empty())
                m_buffer.emplace_back(m_getter());
            else
                m_resetter(Head());
        }
        else
        {
            if (m_buffer.size() < m_capacity && m_headIndex == (m_buffer.size() - 1))
            {
                m_buffer.emplace_back(m_getter());
            }

            //if head catch tail - move head forward
            MoveIndexForward(m_headIndex);
            m_resetter(Head());
            if (m_headIndex == m_tailIndex) MoveIndexForward(m_tailIndex);
        }
    }

    void MoveTailForward()
    {
        if (IsEmpty())
        {
            throw std::out_of_range("buffer is empty");
        }

        m_resetter(Tail());
        if (m_headIndex == m_tailIndex)
        {
            m_empty = true;
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
    Getter<T> m_getter;
    Resetter<T> m_resetter;
    size_t m_capacity;

    size_t m_headIndex = 0;
    size_t m_tailIndex = 0;
    bool m_empty = true;

    CircularBuffer() = delete;
    CircularBuffer(const CircularBuffer<T> &rhs) = delete;
    CircularBuffer(CircularBuffer<T>&& rhs) = delete;
};

}}