#pragma once

#include <vector>

namespace incolun {
namespace clothodb {

template<typename T>
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

    reference MoveForvard();

private:
    std::vector<T> m_buffer;
    size_t m_capacity;
    size_t m_fistItemIndex;
    size_t m_lastItemIndex;
    bool empty = true;

    CircularBuffer() = delete;
};

}}