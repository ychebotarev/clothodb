#pragma once

#include "src/cdb_common/srwlock.h"
#include "src/cdb_common/common.h"
#include "src/cdb_common/sparsepp.h"

#include "ilookup.h"

namespace cdb{
namespace db{

class lookup : public ilookup
{
public:
    lookup();

    virtual cdb::vector_or_error<uint32_t> get_all_uids(
        const std::string& namespace_name,
        const std::string& metric_name,
        const std::map<std::string, std::string>& tags);
    
    virtual cdb::uint_or_error get_uid(
        const std::string& namespace_name,
        const std::string& metric_name, 
        const std::map<std::string, std::string>& tags);
    
    virtual cdb::uint_or_error register_timeseries(
        const std::string& namespace_name,
        const std::string& metric_name, 
        const std::map<std::string, std::string>& tags);
    
    virtual cdb::bool_or_error unregister_timeseries(
        const std::string& namespace_name,
        const std::string& metric_name, 
        const std::map<std::string, std::string>& tags);
    
    virtual uint32_t get_state();

private:
    uint32_t compute_hash(
        const std::string& namespace_name,
        const std::string& metric_name,
        const std::map<std::string, std::string>& tags);
    cdb::uint_or_error get_uid_no_lock(
        const std::string& namespace_name,
        const std::string& metric_name,
        const std::map<std::string, std::string>& tags);


private:
    spp::sparse_hash_map<uint32_t, uint32_t> m_lookup_table;
    
    uint32_t m_last_uid;
    RTL_SRWLOCK m_srw_lock;
};

}}