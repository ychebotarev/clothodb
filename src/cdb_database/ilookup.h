#pragma once

#include "src/cdb_common/srwlock.h"
#include "src/cdb_common/common.h"
#include "src/cdb_common/sparsepp.h"

namespace cdb{
namespace db{

class ilookup
{
public:
    virtual cdb::vector_or_error<uint32_t> get_all_uids(
        const std::string& namespace_name,
        const std::string& metric_name,
        const std::map<std::string, std::string>& tagsMap) = 0;
    
    virtual cdb::uint_or_error get_uid(
        const std::string& namespace_name,
        const std::string& metric_name, 
        const std::map<std::string, std::string>& tagsMap) = 0;
    
    virtual cdb::uint_or_error register_timeseries(
        const std::string& namespace_name,
        const std::string& metric_name, 
        const std::map<std::string, std::string>& tagsMap) = 0;
    
    virtual cdb::bool_or_error unregister_timeseries(
        const std::string& namespace_name,
        const std::string& metric_name, 
        const std::map<std::string, std::string>& tagsMap) = 0;
    
    virtual uint32_t get_state() = 0;
};

}}