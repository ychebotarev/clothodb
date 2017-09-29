#include "src/core/ts_catalog.h"

#include "src/cdb_common/error_codes.h"

#include "src/core/tags_parser.h"
#include "src/core/time_series.h"
#include "src/core/ts_lookup.h"

namespace cdb{
namespace core{

using namespace cdb;
using namespace std;

ts_catalog::ts_catalog()
{
    ::InitializeSRWLock(&m_srw_lock);
    m_lookup = make_shared<ts_lookup>();
}

uint_or_error ts_catalog::create_time_series(
    string metric,
    string tags_str,
    ts_type type,
    ts_scale scale,
    bool store_milliseconds)
{
    tags_map parsed_tags;
    if (!tags_parser::parse(tags_str, parsed_tags))
    {
        return uint_or_error::from_error(ErrorCodes::kFailedToParseTags);
    }
    
    auto propertries = shared_ptr<ts_properties>();
    propertries->m_metric = metric;
    propertries->m_tags.insert(parsed_tags.begin(), parsed_tags.end());
    propertries->m_scale = scale;
    propertries->m_type = type;
    propertries->m_store_milliseconds = store_milliseconds;
    return create_time_series(propertries);
}

uint_or_error ts_catalog::create_time_series(shared_ptr<ts_properties> properties)
{
    SRWLockExclusive lock(m_srw_lock);
    auto uid = m_lookup->find(properties->m_metric, properties->m_tags);
    if (!uid.is_error())
    {
        auto existing_ts = m_timeseries.find(uid.value);
        if (existing_ts != m_timeseries.end())
        {
            //check if it is same timeseries
            auto& existing_properties = *existing_ts->second->get_properties().get();
            if (!properties->compare(existing_properties))
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
    
    auto uid_result = m_lookup->add(properties->m_metric, properties->m_tags);
    if (uid_result.is_error()) 
        return uint_or_error::from_error(uid_result.as_error());
    
    m_timeseries[uid_result.value] = make_shared<time_series>(properties);
    
    return uid_result;
}

bool_or_error ts_catalog::add_value(string metric_name, tags_map tags, uint64_t value, uint64_t timestamp)
{
    SRWLockShared lock(m_srw_lock);
    auto uid = m_lookup->find(metric_name, tags);
    if (uid.is_error())
    {
        return bool_or_error::from_error(ErrorCodes::kTimeSeriesNotFound);
    }

    return add_value(uid.value, value, timestamp);
}

bool_or_error ts_catalog::add_value(uint32_t uid, uint64_t value, uint64_t timestamp)
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

    bool result = ts->add_value(value, timestamp);

    return bool_or_error::from_value(result);
}

uint_or_error ts_catalog::get_uid(string metric_name, tags_map tags)
{
    auto uid = m_lookup->find(metric_name, tags);
    if (uid.is_error())
    {
        return uint_or_error::from_error(ErrorCodes::kTimeSeriesNotFound);
    }

    return uid;
}

bool_or_error ts_catalog::delete_timeseries(string metric_name, tags_map tags)
{
    SRWLockExclusive lock(m_srw_lock);
    uint_or_error uid = m_lookup->find(metric_name, tags);
    if (uid.is_error())
    {
        return bool_or_error::from_error(uid.err);
    }

    m_lookup->remove(metric_name, tags);

    auto ts_iterator = m_timeseries.find(uid.value);
    if (ts_iterator == m_timeseries.end())
        return bool_or_error::from_error(ErrorCodes::kTimeSeriesNotFound);
    m_timeseries.erase(ts_iterator);

    return bool_or_error::from_value(true);
}

}}