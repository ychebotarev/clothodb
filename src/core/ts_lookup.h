#pragma once

#include "src/cdb_common/srwlock.h"
#include "src/cdb_common/common.h"
#include "src/cdb_common/sparsepp.h"

namespace cdb{
namespace core{

class ts_lookup
{
public:
    ts_lookup();

    cdb::vector_or_error<uint32_t> find_all(const std::string& metric_name, cdb::tags_map& tagsMap);
    cdb::uint_or_error find(const std::string& metric_name, cdb::tags_map& tagsMap);
    cdb::uint_or_error add(const std::string& metric_name, cdb::tags_map& tagsMap);
    cdb::bool_or_error remove(const std::string& metric_name, cdb::tags_map& tagsMap);
    
    uint32_t get_state();

    uint32_t compute_hash(const std::string& metric_name, cdb::tags_map& tagsMap);

private:
    spp::sparse_hash_map<uint32_t, uint32_t> m_lookup_table;
    
    uint32_t m_last_uid;
    RTL_SRWLOCK m_srw_lock;
};

}}