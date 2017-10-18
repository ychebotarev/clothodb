#pragma once

#include "src/cdb_common/common.h"

#include "src/cdb_timeseries/ts_type.h"
#include "src/cdb_timeseries/ts_resolution.h"

namespace cdb{
namespace ts{

struct ts_properties
{
    ts_properties()
    {
        m_type = ts_type::TypeInteger;
        m_resolution = ts_resolution::one_sec;
        m_store_milliseconds = false;
    }

    ts_properties(const ts_properties& prop)
    {
        m_type = prop.m_type;
        m_resolution = prop.m_resolution;
        m_store_milliseconds = prop.m_store_milliseconds;
        m_metric = prop.m_metric;
        m_tags.insert(prop.m_tags.begin(), prop.m_tags.end());
    }

    ts_properties(ts_properties&& prop)
    {
        m_type = prop.m_type;
        m_resolution = prop.m_resolution;
        m_store_milliseconds = prop.m_store_milliseconds;
        m_metric = prop.m_metric;
        m_tags.insert(prop.m_tags.begin(), prop.m_tags.end());
    }

    bool compare(ts_properties& to)
    {
        return compare(to.m_metric, to.m_tags);
    }

    bool compare(
        const std::string& metric,
        const std::map<std::string, std::string>& tags)
    {
        if (m_metric != metric) return false;
        if (m_tags.size() != tags.size()) return false;
        const std::map<std::string, std::string>& src_tags = m_tags;
        for (auto it1 = tags.begin(), it2 = src_tags.begin();
            it1 != tags.end();
            ++it1, ++it2)
        {
            if (it1->first != it2->first) return false;
            if (it1->second != it2->second) return false;
        }
        return true;
    }

    ts_type m_type;
    ts_resolution m_resolution;
    bool m_store_milliseconds;

    std::string m_metric;
    std::map<std::string, std::string> m_tags;
};

}}