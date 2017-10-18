#include "command_buffer.h"
#include "command_type.h"
#include "src/cdb_common/serialize_block.h"
#include "src/cdb_common/safe_buffer.h"
#include "src/cdb_storage/iblob.h"

namespace cdb {
namespace svr {

using namespace cdb::sg;
using namespace cdb::ts;
using namespace cdb;
using namespace std;

const size_t default_node_capacity = 32000000;

command_buffer::command_buffer(size_t capacity)
{
    m_active_buffer = std::make_unique<safe_buffer>(capacity);
    m_reserve_buffer = std::make_unique<safe_buffer>(capacity);
}

command_buffer::command_buffer() :command_buffer(default_node_capacity)
{}

command_buffer::~command_buffer()
{}

void command_buffer::log_create_namespace_command(const string& namespace_name)
{
    auto capacity = allocate_string_length(namespace_name) + 1;
    serialize_block block(capacity);
    block.write((char)command_type::create_namespace);
    block.write(namespace_name);
    log_block(block);
}

void command_buffer::log_delete_namespace(const std::string& namespace_name)
{
    auto capacity = allocate_string_length(namespace_name) + 1;
    serialize_block block(capacity);
    block.write((char)command_type::delete_namespace);
    block.write(namespace_name);
    log_block(block);
}

void command_buffer::log_create_time_series(
    const string& namespace_name,
    const string& metric,
    const map<string, string>& tags,
    uint32_t uid,
    ts_type type,
    ts_resolution resolution,
    bool store_milliseconds)
{
    auto capacity = 1 //type
        + allocate_string_length(namespace_name) 
        + allocate_string_length(metric) 
        + sizeof(uint32_t)
        + 1 //size of tags
        + 1 //type
        + 1 //scale
        + 1 //store_milliseconds
        ;

    for (auto& tag: tags)
    {
        capacity += allocate_string_length(tag.first);
        capacity += allocate_string_length(tag.second);
    }

    serialize_block block(capacity);
    block.write((char)command_type::create_timeseries);
    block.write(namespace_name);
    block.write(metric);
    block.write(uid);
    block.write((char)tags.size());
    for (auto& tag : tags)
    {
        block.write(tag.first);
        block.write(tag.second);
    }
    block.write((char)type);
    block.write((char)resolution);
    block.write((char)store_milliseconds);

    log_block(block);
}

void command_buffer::log_delete_timeseries(
    const string& namespace_name,
    const string& metric_name,
    const map<string, string>& tags)
{
    auto capacity = 1 //type
        + allocate_string_length(namespace_name)
        + allocate_string_length(metric_name)
        + 1 //size of tags
        ;

    for (auto& tag : tags)
    {
        capacity += allocate_string_length(tag.first);
        capacity += allocate_string_length(tag.second);
    }

    serialize_block block(capacity);
    block.write((char)command_type::delete_timeseries);
    block.write(namespace_name);
    block.write(metric_name);
    block.write((char)tags.size());
    for (auto& tag : tags)
    {
        block.write(tag.first);
        block.write(tag.second);
    }
    log_block(block);
}

void command_buffer::log_add_value(
    uint32_t uid,
    uint64_t value,
    uint64_t timestamp)
{
    auto capacity = 1 //type
        + sizeof(uint32_t)
        + sizeof(uint64_t)
        + sizeof(uint64_t) //size of tags
        ;
    serialize_block block(capacity);
    block.write((char)command_type::add_value);
    block.write(uid);
    block.write(value);
    block.write(timestamp);
    log_block(block);
}

__forceinline size_t command_buffer::allocate_string_length(const std::string& str)
{
    return str.length() + sizeof(size_t) + 1;
}

void command_buffer::log_block(serialize_block& block)
{
    if (log_to_active_buffer(block)) return;
    std::lock_guard<std::mutex> lock(m_mutex);
    if (log_to_active_buffer(block)) return;

    m_active_buffer.swap(m_reserve_buffer);
    
    log_to_active_buffer(block);
}

__forceinline bool command_buffer::log_to_active_buffer(serialize_block& block)
{
    auto buffer = m_active_buffer->allocate(block.get_capacity());
    if (buffer != nullptr)
    {
        memcpy_s(buffer, block.get_capacity(), block.get_buffer(), block.get_capacity());
        return true;
    }
    return false;
}

}}