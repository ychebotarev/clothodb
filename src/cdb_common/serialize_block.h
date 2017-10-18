#pragma once
#include <cstdint>
#include <memory>
#include <string>

namespace cdb {

//unfortunatly was not able to use Capnproto to serialize log entries
//main reason - Capnproto list doesn't support polimorphic list
//(or I didn't find a way to use it)
class serialize_block
{
public:
    serialize_block(int capacity) : m_capacity(capacity)
    {
        m_buffer = std::make_unique<char[]>(m_capacity);
    }

    void reset()
    {
        m_position = 0;
    }

    bool write(void* data, size_t size);

    template<typename T>
    bool write(T value)
    {
        auto size = sizeof(T);
        if (m_position + size > m_capacity) return false;

        T* position = (T*)&m_buffer[m_position];
        *position = value;
        m_position += size;
        return true;
    }

    bool write(char value);

    bool write(char* string);
    bool write(const std::string& str);
    
    char* get_buffer() { return m_buffer.get(); }
    size_t get_length() const { return m_position; }
    size_t get_capacity() const { return m_capacity; }
private:
    size_t m_position = 0;
    size_t m_capacity;
    std::unique_ptr<char[]> m_buffer;
};

}