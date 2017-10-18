#pragma once

#include "src/cdb_common/common.h"
#include "src/cdb_timeseries/ts_properties.h"
#include "src/cdb_timeseries/data_point.h"

namespace cdb {
namespace db {

class idatabase
{
public:

    virtual ~idatabase()
    {}

    virtual void start() = 0;

    virtual bool_or_error create_namespace(const std::string& namespace_name) = 0;

    virtual bool_or_error delete_namespace(const std::string& namespace_name) = 0;

    virtual uint_or_error create_time_series(
        const std::string& namespace_name,
        const std::string& metric,
        const std::map<std::string, std::string>& tags,
        cdb::ts::ts_type type,
        cdb::ts::ts_resolution resolution,
        bool store_milliseconds) = 0;

    virtual uint_or_error get_uid(
        const std::string& namespace_name,
        const std::string& metric_name,
        const std::map<std::string, std::string>& tags) = 0;
    
    virtual bool_or_error delete_timeseries(
        const std::string& namespace_name,
        const std::string& metric_name,
        const std::map<std::string, std::string>& tags) = 0;

    virtual bool_or_error add_value(
        const std::string& namespace_name,
        const std::string& metric_name,
        const std::map<std::string, std::string>& tags,
        uint64_t value,
        uint64_t timestamp) = 0;

    virtual bool_or_error add_value(
        uint32_t uid,
        uint64_t value,
        uint64_t timestamp) = 0;

    virtual bool_or_error add_value(
        uint32_t uid,
        cdb::ts::data_point dp) 
    {
        return add_value(uid, dp.value, dp.timestamp);
    };

    virtual bool_or_error add_values(
        const std::string& namespace_name,
        const std::string& metric_name,
        const std::map<std::string, std::string>& tags,
        std::vector<cdb::ts::data_point>& values) = 0;

    virtual bool_or_error add_values(
        uint32_t uid,
        std::vector<cdb::ts::data_point>& values) = 0;

    virtual vector_or_error<cdb::ts::data_point> get_points(
        const std::string& namespace_name,
        const std::string& metric_name,
        const std::map<std::string, std::string>& tags) = 0;

    virtual vector_or_error<cdb::ts::data_point> get_points(uint32_t uid) = 0;

    virtual vector_or_error<cdb::ts::data_point> get_points(
        const std::string& namespace_name,
        const std::string& metric_name,
        const std::map<std::string, std::string>& tags,
        uint64_t start_time, 
        uint64_t end_time) = 0;

    virtual vector_or_error<cdb::ts::data_point> get_points(
        uint32_t uid,
        uint64_t start_time, 
        uint64_t end_time) = 0;
};

}}

