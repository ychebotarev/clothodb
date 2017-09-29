#pragma once
#include <cstdint>
#include <memory>
#include <string>

namespace cdb {

class string_buffer
{
public:
    string_buffer(int capacity) : m_capacity(capacity)
    {
        m_buffer = std::make_unique<char[]>(m_capacity);
    }

    void reset()
    {
        m_position = 0;
    }

    bool append(const std::string& str);
    char* get_buffer() { return m_buffer.get(); }
    size_t get_length() const { return m_position; }

private:
    size_t m_position = 0;
    size_t m_capacity;
    std::unique_ptr<char[]> m_buffer;
};

}