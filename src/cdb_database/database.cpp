#include "database.h"
#include "lookup.h"

#include "src/cdb_timeseries/time_series.h"
#include "src/cdb_timeseries/ts_properties.h"
#include "src/cdb_common/error_codes.h"
#include "src/cdb_common/not_implemented_exception.h"

namespace cdb {
namespace db {

using namespace cdb;
using namespace cdb::ts;
using namespace std;

database::database(std::shared_ptr<ilookup> lookup)
{
    ::InitializeSRWLock(&m_srw_lock);
    m_lookup = lookup;
}

void database::start()
{

}

bool_or_error database::create_namespace(const std::string& namespace_name)
{
    SRWLockExclusive lock(m_srw_lock);
    if (has_namespace(namespace_name))
    {
        return bool_or_error::from_error(ErrorCodes::kNamespaceAlreadyExist);
    }

    m_namespaces.insert(namespace_name);

    return bool_or_error::from_value(true);
}

bool_or_error database::delete_namespace(const std::string& namespace_name)
{
    SRWLockExclusive lock(m_srw_lock);
    if (!has_namespace(namespace_name))
    {
        return bool_or_error::from_error(ErrorCodes::kNamespaceNotExist);
    }

    auto iter = m_namespaces.find(namespace_name);
    m_namespaces.erase(iter);

    return bool_or_error::from_value(true);
}

uint_or_error database::create_time_series(
    const string& namespace_name,
    const string& metric,
    const map<string, string>& tags,
    ts_type type,
    ts_resolution resolution,
    bool store_milliseconds)
{
    SRWLockExclusive lock(m_srw_lock);

    //namespace must exist
    if (!has_namespace(namespace_name))
    {
        return uint_or_error::from_error(ErrorCodes::kNamespaceNotExist);
    }
    
    //timeseries with corresponding uid shoud NOT exist
    auto uid = m_lookup->get_uid(namespace_name, metric, tags);
    if (!uid.is_error())
    {
        // if uid exist there are two possible cases:
        // a. there is timeseties with the same metric/tags
        // b. hash collision

        auto existing_ts = m_timeseries.find(uid.value);
        if (existing_ts != m_timeseries.end())
        {
            //check if it is same timeseries
            auto& existing_properties = existing_ts->second->get_properties();
            if (!existing_properties->compare(metric, tags))
            {
                return uint_or_error::from_error(ErrorCodes::kHashCollision);
            }
            return uint_or_error::from_error(ErrorCodes::kTimeSeriesAlreadyExist);
        }
        else
        {
            //there is no timeseries but entry exist in lookup table
            return uint_or_error::from_error(ErrorCodes::kLookupCollision);
        }
    }

    auto properties = shared_ptr<ts_properties>();
    properties->m_metric = metric;
    properties->m_tags.insert(tags.begin(), tags.end());
    properties->m_resolution = resolution;
    properties->m_type = type;
    properties->m_store_milliseconds = store_milliseconds;

    auto uid_result = m_lookup->register_timeseries(
        namespace_name,
        properties->m_metric, 
        properties->m_tags);
    
    if (uid_result.is_error())
        return uint_or_error::from_error(uid_result.as_error());

    m_timeseries[uid_result.value] = make_shared<time_series>(properties);

    return uid_result;
}

bool_or_error database::delete_timeseries(
    const string& namespace_name,
    const string& metric_name,
    const map<string, string>& tags)
{
    SRWLockExclusive lock(m_srw_lock);
    uint_or_error uid = m_lookup->get_uid(namespace_name, metric_name, tags);
    if (uid.is_error())
    {
        return bool_or_error::from_error(uid.err);
    }

    m_lookup->unregister_timeseries(namespace_name, metric_name, tags);

    auto ts_iterator = m_timeseries.find(uid.value);
    if (ts_iterator == m_timeseries.end())
        return bool_or_error::from_error(ErrorCodes::kTimeSeriesNotFound);
    m_timeseries.erase(ts_iterator);

    return bool_or_error::from_value(true);
}

bool_or_error database::add_value(
    const string& namespace_name,
    const string& metric_name,
    const map<string, string>& tags,
    cdb::ts::data_point& point)
{
    return add_value(namespace_name, metric_name, tags, point.value, point.timestamp);
}

bool_or_error database::add_value(
    const string& namespace_name,
    const string& metric_name,
    const map<string, string>& tags,
    uint64_t value,
    uint64_t timestamp)
{
    auto result = m_lookup->get_uid(namespace_name, metric_name, tags);;
    if (result.is_error())
    {
        return bool_or_error::from_error(result.as_error());
    }
    auto uid = result.as_value();
    return add_value(uid, value, timestamp);
}

bool_or_error database::add_value(
    uint32_t uid,
    uint64_t value,
    uint64_t timestamp)
{
    auto ts = get_time_series(uid);
    if (ts.is_error())
    {
        return bool_or_error::from_error(ts.as_error());
    }

    bool result = ts.value->add_value(value, timestamp);
    
    return bool_or_error::from_value(result);
}

bool_or_error database::add_values(
    const string& namespace_name,
    const string& metric_name,
    const map<string, string>& tags,
    vector<data_point>& values)
{
    auto result = m_lookup->get_uid(namespace_name, metric_name, tags);
    if (result.is_error())
    {
        return bool_or_error::from_error(result.as_error());
    }
    auto uid = result.as_value();
    return add_values(uid, values);
}

bool_or_error database::add_values(
    uint32_t uid,
    vector<data_point>& values)
{
    shared_ptr<time_series> ts;
    {
        SRWLockShared lock(m_srw_lock);
        auto ts_iterator = m_timeseries.find(uid);
        if (ts_iterator == m_timeseries.end())
            return bool_or_error::from_error(ErrorCodes::kTimeSeriesNotFound);
        ts = ts_iterator->second;
    }

    if (ts.get() == 0)
        return bool_or_error::from_error(ErrorCodes::kTimeSeriesNotFound);
    
    bool result = true;
    for (auto& point : values)
    {
        result &= ts->add_value(point.value, point.timestamp);
    }

    return bool_or_error::from_value(result);
}

vector_or_error<data_point> database::get_points(
    const string& namespace_name,
    const string& metric_name,
    const map<string, string>& tags)
{
    throw cdb::not_implemented("local_storage::delete_blob");
}

vector_or_error<data_point> database::get_points(uint32_t uid)
{
    throw cdb::not_implemented("local_storage::delete_blob");
}

vector_or_error<data_point> database::get_points(
    const string& namespace_name,
    const string& metric_name,
    const map<string, string>& tags,
    uint64_t start_time,
    uint64_t end_time)
{
    throw cdb::not_implemented("local_storage::delete_blob");
}

vector_or_error<data_point> database::get_points(
    uint32_t uid,
    uint64_t start_time,
    uint64_t end_time)
{
    throw cdb::not_implemented("local_storage::delete_blob");
}

uint_or_error database::get_uid(
    const string& namespace_name,
    const string& metric_name,
    const map<string, string>& tags)
{
    return m_lookup->get_uid(namespace_name, metric_name, tags);
}

pointer_or_error<time_series> database::get_time_series(uint32_t uid)
{
    SRWLockShared lock(m_srw_lock);
    auto ts_iterator = m_timeseries.find(uid);
    if (ts_iterator == m_timeseries.end())
        return pointer_or_error<time_series>::from_error(ErrorCodes::kTimeSeriesNotFound);

    return pointer_or_error<time_series>::from_value(ts_iterator->second);
}

__forceinline bool database::has_namespace(const std::string& namespace_name)
{
    auto iter = m_namespaces.find(namespace_name);
    return (iter != m_namespaces.end());
}

}}