#pragma once

#include <vector>

namespace cdb {

//T -type name
//C - creation function
//R - reset fucntion
template<class T>
class circular_buffer
{
public:
    template<typename T> using item_getter = T(*)();
    template<typename T> using item_resetter = void(*)(T&);

    circular_buffer(size_t capacity, item_getter<T> getter, item_resetter<T> resetter) :
        m_capacity(capacity),
        m_getter(getter),
        m_resetter(resetter)
    {}

    T& operator[](size_t pos)
    {
        return At(pos);
    }

    T& at(size_t pos)
    {
        if(pos >= m_capacity)
        {
            throw std::out_of_range("index our of range");
        }
        if (m_head_index >= m_tail_index &&
            (pos < m_tail_index || pos > m_head_index))
        {
            throw std::out_of_range("index our of range");
        }

        auto p = (first + pos) % buffer.size();
        return buffer[p];
    }

    bool is_empty() const { return m_empty; }

    size_t size() const
    {
        if (is_empty()) return 0;
        return (m_head_index >= m_tail_index) ?
            (m_head_index - m_tail_index + 1) :
            (m_capacity - m_tail_index + m_head_index + 1);
    }

    void set_head_index(size_t index)
    {
        m_empty = false;
        if (index >= m_capacity) index = index % m_capacity;
        m_head_index = index;
    }

    void set_tail_index(size_t index)
    {
        m_empty = false;
        if (index >= m_capacity) index = index % m_capacity;
        m_tail_index = index;
    }

    T& head()
    {
        if (is_empty())
        {
            throw std::out_of_range("buffer is empty");
        }
        return m_buffer[m_head_index];
    }

    T& tail()
    {
        if (is_empty())
        {
            throw std::out_of_range("buffer is empty");
        }
        return m_buffer[m_tail_index];
    }

    void move_head_forward()
    {
        if (is_empty())
        {
            m_empty = false;
            m_head_index = 0;
            m_tail_index = 0;

            if (m_buffer.empty())
                m_buffer.emplace_back(m_getter());
            else
                m_resetter(head());
        }
        else
        {
            if (m_buffer.size() < m_capacity && m_head_index == (m_buffer.size() - 1))
            {
                m_buffer.emplace_back(m_getter());
            }

            //if head catch tail - move head forward
            move_index_forward(m_head_index);
            m_resetter(head());
            if (m_head_index == m_tail_index) 
                move_index_forward(m_tail_index);
        }
    }

    void move_tail_forward()
    {
        if (is_empty())
        {
            throw std::out_of_range("buffer is empty");
        }

        m_resetter(tail());
        if (m_head_index == m_tail_index)
        {
            m_empty = true;
        }
        move_index_forward(m_tail_index);
    }

private:
    void move_index_forward(size_t& index) const
    {
        ++index;
        if (index >= m_capacity) index = index % m_capacity;
    }

private:
    std::vector<T> m_buffer;
    item_getter<T> m_getter;
    item_resetter<T> m_resetter;
    size_t m_capacity;

    size_t m_head_index = 0;
    size_t m_tail_index = 0;
    bool m_empty = true;

    circular_buffer() = delete;
    circular_buffer(const circular_buffer<T> &rhs) = delete;
    circular_buffer(circular_buffer<T>&& rhs) = delete;
};

}