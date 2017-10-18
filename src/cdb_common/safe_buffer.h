#pragma once

#include <memory>

namespace cdb {

class safe_buffer
{
public:
    safe_buffer(size_t capacity);

    void reset()
    {
        m_position = 0;
    }

    bool write(void* data, size_t size);
    char* allocate(size_t size);
    __forceinline bool has_capacity(size_t size) { return m_position + size > m_capacity; }
    
    char* get_buffer() { return m_buffer.get(); }
    size_t get_length() const { return m_position; }

private:
    size_t m_position = 0;
    size_t m_capacity;
    std::unique_ptr<char[]> m_buffer;
};

}
