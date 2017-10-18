#include "server.h"

#include "command_type.h"
#include "command_buffer.h"
#include "command_replay.h"

#include "src/cdb_common/error_codes.h"
#include "src/cdb_database/database.h"
#include "src/cdb_database/lookup.h"

#include "src/cdb_storage/local_storage.h"
#include "src/cdb_storage/iblob.h"

using namespace std;
using namespace cdb::db;
using namespace cdb::sg;
using namespace cdb::ts;

namespace cdb {
namespace svr{

server::server()
{
    m_command_buffer = std::make_unique<command_buffer>();
    m_storage = std::unique_ptr<local_storage>(new local_storage());
    
    m_lookup = std::make_shared<lookup>();
    m_database = std::shared_ptr<idatabase>(new database(m_lookup));
}

server::~server()
{}

void server::load()
{
    {
        auto blob = m_storage->get_blob("test");
        auto data = blob->get_data();
        command_replay commandReplay(data);
        commandReplay.replay(m_database, m_lookup);
    }
}


bool_or_error server::create_namespace(std::string namespace_name)
{
    if (namespace_name.empty()) return bool_or_error::from_error(ErrorCodes::kInvalidArguments);
    
    auto result = m_database->create_namespace(namespace_name);
    if (result.is_error()) return result;

    m_command_buffer->log_create_namespace_command(namespace_name);

    return result;
}

bool_or_error server::delete_namespace(std::string namespace_name)
{
    return bool_or_error::from_error(ErrorCodes::kNotImplemented);
}

uint_or_error server::create_time_series(
    string namespace_name,
    string metric,
    map<string, string> tags,
    ts_type type,
    ts_resolution resolution,
    bool store_milliseconds)
{
    if (namespace_name.empty()
        || metric.empty()
        || tags.size() > 5)
    {
        return uint_or_error::from_error(ErrorCodes::kInvalidArguments);
    }

    auto result = m_database->create_time_series(
        namespace_name,
        metric, tags,
        type, resolution, store_milliseconds)
        ;
    if (result.is_error()) return result;

    m_command_buffer->log_create_time_series(
        namespace_name, metric, tags,
        result.as_value(),
        type, resolution, store_milliseconds);
    return result;
}

uint_or_error server::get_uid(
    string namespace_name,
    string metric_name,
    map<string, string> tags)
{
    return uint_or_error::from_error(ErrorCodes::kNotImplemented);
}

bool_or_error server::delete_timeseries(
    string namespace_name,
    string metric_name,
    map<string, string> tags)
{
    return bool_or_error::from_error(ErrorCodes::kNotImplemented);
}

bool_or_error server::add_value(
    string namespace_name,
    string metric_name,
    map<string, string> tags,
    uint64_t value,
    uint64_t timestamp)
{
    return bool_or_error::from_error(ErrorCodes::kNotImplemented);
}

bool_or_error server::add_value(
    uint32_t uid,
    uint64_t value,
    uint64_t timestamp)
{
    return bool_or_error::from_error(ErrorCodes::kNotImplemented);
}

bool_or_error server::add_values(
    string namespace_name,
    string metric_name,
    map<string, string> tags,
    std::vector<cdb::ts::data_point>& values)
{
    return bool_or_error::from_error(ErrorCodes::kNotImplemented);
}

bool_or_error server::add_values(
    uint32_t uid,
    std::vector<cdb::ts::data_point>& values)
{
    return bool_or_error::from_error(ErrorCodes::kNotImplemented);
}

}}