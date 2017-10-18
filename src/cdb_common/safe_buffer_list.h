#pragma once

#include <memory>
#include <mutex>

namespace cdb {

class safe_buffer;

struct safe_buffer_node
{
    std::unique_ptr<safe_buffer> buffer;
    safe_buffer_node* next;
};

class safe_buffer_list
{
public:
    safe_buffer_list(size_t node_capacity);
    ~safe_buffer_list();

    bool write(void* data, size_t size);
    char* allocate(size_t size);

    safe_buffer_node* detach();

private:
    safe_buffer_node* set_head(safe_buffer_node* head);
    safe_buffer_node* get_tail(safe_buffer_node* head);
    void free_safe_buffer_list(safe_buffer_node* head);
    void cleanup();
    void initialize();

private:
    safe_buffer_node* m_head;
    safe_buffer_node* m_tail;
    size_t m_node_capacity;
    std::mutex m_mutex;
    
};

}
