#pragma once

#include "src/cdb_database/idatabase.h"
#include "src/cdb_common/srwlock.h"
#include "src/cdb_common/sparsepp.h"

namespace cdb {
namespace ts {

class time_series;

}}

namespace cdb {
namespace db {

class ilookup;

class database : public idatabase
{
public:
    database(std::shared_ptr<ilookup>);

    virtual void start();

    virtual bool_or_error create_namespace(const std::string& namespace_name);
    
    virtual bool_or_error delete_namespace(const std::string& namespace_name);

    virtual uint_or_error create_time_series(
        const std::string& namespace_name,
        const std::string& metric,
        const std::map<std::string, std::string>& tags,
        cdb::ts::ts_type type,
        cdb::ts::ts_resolution resolution,
        bool store_milliseconds);

    virtual uint_or_error get_uid(
        const std::string& namespace_name,
        const std::string& metric_name,
        const std::map<std::string, std::string>& tags);

    virtual bool_or_error delete_timeseries(
        const std::string& namespace_name,
        const std::string& metric_name,
        const std::map<std::string, std::string>& tags);

    virtual bool_or_error add_value(
        const std::string& namespace_name,
        const std::string& metric_name,
        const std::map<std::string, std::string>& tags,
        uint64_t value,
        uint64_t timestamp);

    virtual bool_or_error add_value(
        const std::string& namespace_name,
        const std::string& metric_name,
        const std::map<std::string, std::string>& tags,
        cdb::ts::data_point& point);

    virtual bool_or_error add_value(
        uint32_t uid,
        uint64_t value,
        uint64_t timestamp);

    virtual bool_or_error add_values(
        const std::string& catalog_name,
        const std::string& metric_name,
        const std::map<std::string, std::string>& tags,
        std::vector<cdb::ts::data_point>& values);

    virtual bool_or_error add_values(
        uint32_t uid,
        std::vector<cdb::ts::data_point>& values);

    virtual vector_or_error<cdb::ts::data_point> get_points(
        const std::string& namespace_name,
        const std::string& metric_name,
        const std::map<std::string, std::string>& tags);

    virtual vector_or_error<cdb::ts::data_point> get_points(uint32_t uid);

    virtual vector_or_error<cdb::ts::data_point> get_points(
        const std::string& namespace_name,
        const std::string& metric_name,
        const std::map<std::string, std::string>& tags,
        uint64_t start_time,
        uint64_t end_time);

    virtual vector_or_error<cdb::ts::data_point> get_points(
        uint32_t uid,
        uint64_t start_time,
        uint64_t end_time);
private:
    cdb::pointer_or_error<cdb::ts::time_series> get_time_series(uint32_t uid);
    bool has_namespace(const std::string& namespace_name);
private:
    spp::sparse_hash_map<uint32_t, std::shared_ptr<cdb::ts::time_series>> m_timeseries;
    spp::sparse_hash_set<std::string> m_namespaces;
    std::shared_ptr<ilookup> m_lookup;
    RTL_SRWLOCK m_srw_lock;
};

}}

