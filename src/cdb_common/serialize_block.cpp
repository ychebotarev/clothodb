#include "serialize_block.h"

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

namespace cdb{

//strings are stored in both forms
//string length is stored first
//then _null terminated_ string is stored
bool serialize_block::write(char* str_ptr)
{
    auto length = strlen(str_ptr);
    if (length == 0) return false;

    if (m_position + length + sizeof(size_t) + 1 > m_capacity) return false;
    write(&length, sizeof(size_t));
    write(str_ptr, length);
    m_buffer[m_position] = 0;
    ++m_position;
    return true;
}

bool serialize_block::write(const std::string& str)
{
    auto length = str.length();
    if (m_position + length + sizeof(size_t) + 1 > m_capacity) return false;
    write(&length, sizeof(size_t));
    write((void*)str.c_str(), str.length());
    m_buffer[m_position] = 0;
    ++m_position;
    return true;
}

bool serialize_block::write(void* data, size_t size)
{
    if (m_position + size > m_capacity) return false;
    char* buffer = &m_buffer[m_position];
    memcpy_s(buffer + m_position, m_capacity - m_position, data, size);
    m_position += size;
    return true;
}

//template<typename T>
//bool serialize_block::write(T value)
//{
//    auto size = sizeof(T);
//    if (m_position + size > m_capacity) return false;
//
//    T* position = (T*)&m_buffer[m_position];
//    *position = value;
//    m_position += size;
//    return true;
//}

bool serialize_block::write(char value)
{
    if (m_position >= m_capacity) return false;

    m_buffer[m_position] = value;
    ++m_position;
    return true;
}

}