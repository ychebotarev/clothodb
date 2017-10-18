#pragma once

#include "src/cdb_common/common.h"
#include "src/cdb_timeseries/ts_properties.h"
#include "src/cdb_timeseries/data_point.h"

/////////////////////////////////////////////////
//forward declarations
namespace cdb { namespace db {
    class idatabase;
    class ilookup;
}}

namespace cdb { namespace sg {
    class istorage;
}}
/////////////////////////////////////////////////

namespace cdb {
namespace svr{

class command_buffer;

class server
{
public:
    server();
    ~server();

    void load();

    virtual bool_or_error create_namespace(std::string namespace_name);

    virtual bool_or_error delete_namespace(std::string namespace_name);

    virtual uint_or_error create_time_series(
        std::string namespace_name,
        std::string metric,
        std::map<std::string, std::string> tags,
        cdb::ts::ts_type type,
        cdb::ts::ts_resolution resolution,
        bool store_milliseconds);

    virtual uint_or_error get_uid(
        std::string namespace_name,
        std::string metric_name,
        std::map<std::string, std::string> tags);

    virtual bool_or_error delete_timeseries(
        std::string namespace_name,
        std::string metric_name,
        std::map<std::string, std::string> tags);

    virtual bool_or_error add_value(
        std::string namespace_name,
        std::string metric_name,
        std::map<std::string, std::string> tags,
        uint64_t value,
        uint64_t timestamp);

    virtual bool_or_error add_value(
        uint32_t uid,
        uint64_t value,
        uint64_t timestamp);

    virtual bool_or_error add_value(
        uint32_t uid,
        cdb::ts::data_point dp)
    {
        return add_value(uid, dp.value, dp.timestamp);
    };

    virtual bool_or_error add_values(
        std::string namespace_name,
        std::string metric_name,
        std::map<std::string, std::string> tags,
        std::vector<cdb::ts::data_point>& values);

    virtual bool_or_error add_values(
        uint32_t uid,
        std::vector<cdb::ts::data_point>& values) ;
private:
    std::unique_ptr<command_buffer> m_command_buffer;
    std::unique_ptr<cdb::sg::istorage> m_storage;
    std::shared_ptr<cdb::db::idatabase> m_database;
    std::shared_ptr<cdb::db::ilookup> m_lookup;
};

}}
