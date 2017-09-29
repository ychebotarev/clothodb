#include "ts_lookup.h"
#include "tags_parser.h"

#include "src/cdb_common/string_buffer.h"
#include "src/cdb_common/MurmurHash3.h"
#include "src/cdb_common/error_codes.h"

namespace cdb {
namespace core {

using namespace cdb;
using namespace std;

const int max_hash_bufer_len = 1024;

ts_lookup::ts_lookup()
{
    ::InitializeSRWLock(&m_srw_lock);
    m_last_uid = 0;
}

uint32_t ts_lookup::get_state()
{
    SRWLockShared lock(m_srw_lock);
    return m_last_uid;
}

vector_or_error<uint32_t> ts_lookup::find_all(const string& metric_name, tags_map& tagsMap)
{
    return vector_or_error<uint32_t>::from_error(ErrorCodes::kNotImplemented);
}

uint_or_error ts_lookup::find(const string& metric_name, tags_map& tagsMap)
{
    uint32_t uid = 0;
    {
        SRWLockShared lock(m_srw_lock);
        auto hash = compute_hash(metric_name, tagsMap);
        const auto uid_ptr = m_lookup_table.find(hash);
        if (uid_ptr == m_lookup_table.end())
            return uint_or_error::from_error(ErrorCodes::kTimeSeriesNotFound);
        uid = uid_ptr->second;
    }
    return uint_or_error::from_value(uid);
}

uint_or_error ts_lookup::add(const string& metric_name, tags_map& tagsMap)
{
    auto existing_guid = find(metric_name, tagsMap);
    if (!existing_guid.is_error()) 
        return uint_or_error::from_error(ErrorCodes::kLookupMetricAlreadyExist);

    SRWLockExclusive lock(m_srw_lock);

    //can't call find, lock is not recursive
    auto hash = compute_hash(metric_name, tagsMap);
    const auto uid_ptr = m_lookup_table.find(hash);
    if (uid_ptr != m_lookup_table.end())
        return uint_or_error::from_error(ErrorCodes::kLookupMetricAlreadyExist);

    ++m_last_uid;
    m_lookup_table[hash] = m_last_uid;

    return uint_or_error::from_value(m_last_uid);
}

bool_or_error ts_lookup::remove(const string& metric_name, tags_map& tagsMap)
{
    auto existing_guid = find(metric_name, tagsMap);
    if (!existing_guid.is_error())
        return bool_or_error::from_error(ErrorCodes::kLookupMetricAlreadyExist);

    SRWLockExclusive lock(m_srw_lock);
    //double check
    //can't call find, lock is not recursive
    auto hash = compute_hash(metric_name, tagsMap);
    const auto uid_ptr = m_lookup_table.find(hash);
    if (uid_ptr == m_lookup_table.end())
        return bool_or_error::from_error(ErrorCodes::kTimeSeriesNotFound);
    m_lookup_table.erase(uid_ptr);
    return bool_or_error::from_value(true);
}

uint32_t ts_lookup::compute_hash(const string& metric_name, tags_map& tagsMap)
{
    size_t capacity = 1;
    uint32_t hash = 0;
    MurmurHash3_x86_32(metric_name.c_str(), (int)metric_name.length(), hash, &hash);

    for (auto& tag : tagsMap)
    {
        MurmurHash3_x86_32(tag.first.c_str(), (int)tag.first.length(), hash, &hash);
        MurmurHash3_x86_32(tag.second.c_str(), (int)tag.second.length(), hash, &hash);
    }
    return hash;
}

}}