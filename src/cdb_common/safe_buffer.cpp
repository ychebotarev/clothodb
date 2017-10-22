#include "safe_buffer.h"
#include <windows.h>

namespace cdb {

safe_buffer::safe_buffer(size_t capacity)
    :m_capacity(capacity)
{
    m_buffer = std::make_unique<char[]>(m_capacity);
}

bool safe_buffer::write(void* data, size_t size)
{
    for(;;)
    {
        auto old_position = m_position;
        auto expected_position = old_position + size;
        if (expected_position> m_capacity) return false;
        auto current_position = 
            ::InterlockedCompareExchange(&m_position, expected_position, old_position);
        if (current_position == old_position) break;
    }
    char* buffer = &m_buffer[m_position];
    memcpy_s(buffer, m_capacity - m_position, data, size);
    return true;
}

char* safe_buffer::allocate(size_t size)
{
    for (;;)
    {
        auto old_position = m_position;
        auto expected_position = old_position + size;
        if (expected_position > m_capacity) return nullptr;
        auto current_position =
            ::InterlockedCompareExchange(&m_position, expected_position, old_position);
        if (current_position == old_position)
        {
            char* buffer = &m_buffer[old_position];
            return buffer;

        };
    }
}


}
