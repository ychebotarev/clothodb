#pragma once

#include <string>
#include <memory>
#include <mutex>

#include "src/cdb_common/common.h"
#include "src/cdb_timeseries/ts_resolution.h"
#include "src/cdb_timeseries/ts_type.h"

////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////
namespace cdb {
    class safe_buffer;
    class serialize_block;
}

namespace cdb { namespace sg {
    class iblob;
}}
///////////////////////////////////////////


namespace cdb {
namespace svr {


class command_buffer
{
public:
    command_buffer();
    command_buffer(size_t capacity);

    ~command_buffer();

    void log_create_namespace_command(const std::string& namespace_name);
    void log_delete_namespace(const std::string& namespace_name);

    void log_create_time_series(
        const std::string& namespace_name,
        const std::string& metric,
        const std::map<std::string, std::string>& tags,
        uint32_t uid,
        cdb::ts::ts_type type,
        cdb::ts::ts_resolution resolution,
        bool store_milliseconds);

    void log_delete_timeseries(
        const std::string& namespace_name,
        const std::string& metric_name,
        const std::map<std::string, std::string>& tags);

    void log_add_value(
        uint32_t uid,
        uint64_t value,
        uint64_t timestamp);
private:
    size_t allocate_string_length(const std::string& str);
    void log_block(serialize_block& block);
    bool log_to_active_buffer(serialize_block& block);
private:
    std::unique_ptr<safe_buffer> m_active_buffer;
    std::unique_ptr<safe_buffer> m_reserve_buffer;

    std::mutex m_mutex;
};

}}