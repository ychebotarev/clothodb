#pragma once

#include <vector>
#include "src/cdb_timeseries/data_point.h"

class data_point_provider
{
public:
    static std::vector<cdb::ts::data_point>& get_points();

private:
    std::vector<cdb::ts::data_point> m_expected_points;

};

