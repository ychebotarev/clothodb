#include "string_buffer.h"

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

namespace cdb{

bool string_buffer::append(const std::string& str)
{
    auto append_len = str.length();
    if (m_position + append_len >= m_capacity) return false;
    char* buffer = &m_buffer[0];
    memcpy_s(buffer + m_position, m_capacity - m_position, str.c_str(), append_len);
    m_position += append_len;
    m_buffer[m_position] = 0;
    return true;
}

}