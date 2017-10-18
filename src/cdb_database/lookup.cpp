#include "lookup.h"
#include "tags_parser.h"

#include "src/cdb_common/MurmurHash3.h"
#include "src/cdb_common/error_codes.h"

namespace cdb {
namespace db {

using namespace cdb;
using namespace std;

const int max_hash_bufer_len = 1024;

lookup::lookup()
{
    ::InitializeSRWLock(&m_srw_lock);
    m_last_uid = 0;
}

uint32_t lookup::get_state()
{
    SRWLockShared lock(m_srw_lock);
    return m_last_uid;
}

vector_or_error<uint32_t> lookup::get_all_uids(
    const string& namespace_name, 
    const string& metric_name, 
    const map<string, string>& tags)
{
    return vector_or_error<uint32_t>::from_error(ErrorCodes::kNotImplemented);
}

uint_or_error lookup::get_uid(
    const string& namespace_name,
    const string& metric_name, 
    const map<string, string>& tags)
{
    SRWLockShared lock(m_srw_lock);
    return get_uid_no_lock(namespace_name, metric_name, tags);
}
uint_or_error lookup::get_uid_no_lock(
    const string& namespace_name,
    const string& metric_name,
    const map<string, string>& tags)
{
    auto hash = compute_hash(namespace_name, metric_name, tags);
    const auto uid_ptr = m_lookup_table.find(hash);
    if (uid_ptr == m_lookup_table.end())
        return uint_or_error::from_error(ErrorCodes::kTimeSeriesNotFound);
    return uint_or_error::from_value(uid_ptr->second);
}


uint_or_error lookup::register_timeseries(
    const string& namespace_name, 
    const string& metric_name, 
    const map<string, string>& tags)
{
    auto existing_guid = get_uid(namespace_name, metric_name, tags);
    if (!existing_guid.is_error()) 
        return uint_or_error::from_error(ErrorCodes::kLookupMetricAlreadyExist);

    SRWLockExclusive lock(m_srw_lock);

    auto hash = compute_hash(namespace_name, metric_name, tags);
    const auto uid_ptr = m_lookup_table.find(hash);
    if (uid_ptr != m_lookup_table.end())
        return uint_or_error::from_error(ErrorCodes::kLookupMetricAlreadyExist);

    ++m_last_uid;
    m_lookup_table[hash] = m_last_uid;

    return uint_or_error::from_value(m_last_uid);
}

bool_or_error lookup::unregister_timeseries(
    const string& namespace_name, 
    const string& metric_name, 
    const map<string, string>& tags)
{
    auto existing_guid = get_uid(namespace_name, metric_name, tags);
    if (existing_guid.is_error())
        return bool_or_error::from_error(ErrorCodes::kLookupMetricNotFound);

    SRWLockExclusive lock(m_srw_lock);

    auto hash = compute_hash(namespace_name, metric_name, tags);
    const auto uid_ptr = m_lookup_table.find(hash);
    if (uid_ptr == m_lookup_table.end())
        return bool_or_error::from_error(ErrorCodes::kTimeSeriesNotFound);
    m_lookup_table.erase(uid_ptr);
    return bool_or_error::from_value(true);
}

uint32_t lookup::compute_hash(
    const string& namespace_name, 
    const string& metric_name, 
    const map<string, string>& tags)
{
    size_t capacity = 1;
    uint32_t hash = 0;
    MurmurHash3_x86_32(namespace_name.c_str(), (int)metric_name.length(), hash, &hash);
    MurmurHash3_x86_32(metric_name.c_str(), (int)metric_name.length(), hash, &hash);

    for (auto& tag : tags)
    {
        MurmurHash3_x86_32(tag.first.c_str(), (int)tag.first.length(), hash, &hash);
        MurmurHash3_x86_32(tag.second.c_str(), (int)tag.second.length(), hash, &hash);
    }
    return hash;
}

}}