#pragma once

#include <string>

#include "src/core/ts_type.h"
#include "src/core/ts_resolution.h"

namespace clothodb {
namespace core {

struct ts_properties
{
    ts_properties()
    {
        m_type = ts_type::TypeInteger;
        m_store_milliseconds = false;
    }

    ts_properties(ts_properties& properties)
    {
        m_type = properties.m_type;
        m_store_milliseconds = properties.m_store_milliseconds;
        m_name = properties.m_name;
        m_tag1 = properties.m_tag1;
        m_tag2 = properties.m_tag2;
        m_tag3 = properties.m_tag3;
        m_tag4 = properties.m_tag4;
        m_tag5 = properties.m_tag5;
    }

    ts_type m_type;
    ts_resolution m_resolution;
    bool m_store_milliseconds;

    std::string m_name;

    std::string m_tag1;
    std::string m_tag2;
    std::string m_tag3;
    std::string m_tag4;
    std::string m_tag5;
};

}}
