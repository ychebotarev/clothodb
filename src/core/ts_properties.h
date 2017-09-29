#pragma once

#include "src/cdb_common/common.h"

#include "src/core/ts_type.h"
#include "src/core/ts_scale.h"

namespace cdb{
namespace core{

struct ts_properties
{
    ts_properties()
    {
        m_type = ts_type::TypeInteger;
        m_scale= ts_scale::one_sec;
        m_store_milliseconds = false;
    }

    ts_properties(ts_properties& properties)
    {
        m_metric = properties.m_metric;
        m_tags.insert(properties.m_tags.begin(), properties.m_tags.end());

        m_type = properties.m_type;
        m_scale = properties.m_scale;
        m_store_milliseconds = properties.m_store_milliseconds;  
    }

    bool compare(ts_properties& to)
    {
        if (m_metric != to.m_metric) return false;
        if (m_tags.size() != to.m_tags.size()) return false;
        for (auto it1 = m_tags.begin(), it2 = to.m_tags.begin(); it1 != m_tags.end(); ++it1, ++it2)
        {
            if (it1->first != it2->first) return false;
            if (it1->second != it2->second) return false;
        }
        return true;
    }

    ts_type m_type;
    ts_scale m_scale;
    bool m_store_milliseconds;

    std::string m_metric;

    tags_map m_tags;
};

}}