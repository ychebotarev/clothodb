#include <windows.h>

#include "safe_buffer.h"
#include "safe_buffer_list.h"

namespace cdb {

safe_buffer_list::safe_buffer_list(size_t node_capacity)
    : m_node_capacity(node_capacity)
{
    initialize();
}


safe_buffer_list::~safe_buffer_list()
{
    cleanup();
}

void safe_buffer_list::initialize()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_head != nullptr) return;
    
    auto head = new safe_buffer_node;
    head->buffer = std::make_unique<safe_buffer>(m_node_capacity);
    head->next = nullptr;
    m_head = head;
    m_tail = head;
}

char* safe_buffer_list::allocate(size_t size)
{
    if (m_tail == nullptr)
    {
        initialize();
    }

    auto buffer = m_tail->buffer->allocate(size);
    if (buffer != nullptr) return buffer;

    std::lock_guard<std::mutex> lock(m_mutex);
    buffer = m_tail->buffer->allocate(size);
    if (buffer != nullptr) return buffer;

    auto new_tail = new safe_buffer_node;
    new_tail->buffer = std::make_unique<safe_buffer>(m_node_capacity);
    new_tail->next = nullptr;

    m_tail->next = new_tail;
    m_tail = new_tail;
    buffer = m_tail->buffer->allocate(size);
    return buffer;
}

bool safe_buffer_list::write(void* data, size_t size)
{
    auto buffer = allocate(size);
    if (buffer == nullptr) return false;
    memcpy_s(buffer, size, data, size);
    return true;
}

// detach existing node head
// new node head will be created
safe_buffer_node* safe_buffer_list::detach()
{
    safe_buffer_node* new_head = new safe_buffer_node;
    new_head->next = 0;
    new_head->buffer = std::make_unique<safe_buffer>(m_node_capacity);

    auto old_head = set_head(new_head);
    return old_head;
}

void safe_buffer_list::cleanup()
{
    safe_buffer_node* head = set_head(nullptr);
    free_safe_buffer_list(head);
}

void safe_buffer_list::free_safe_buffer_list(safe_buffer_node* head)
{
    while (head != nullptr)
    {
        auto temp = head->next;
        delete head;
        head = temp;
    }
}

safe_buffer_node* safe_buffer_list::get_tail(safe_buffer_node* head)
{
    if (head == nullptr) return nullptr;
    while (head->next != nullptr) head = head->next;
    return head;
}

safe_buffer_node* safe_buffer_list::set_head(safe_buffer_node* new_head)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    safe_buffer_node* old_head = m_head;
    m_head = new_head;
    m_tail = get_tail(m_head);
    return old_head;
}

}