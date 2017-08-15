#pragma once

#include <cstdint>

namespace incolun {
namespace clothodb {

class TimeSeriesPoint
{
public:
    TimeSeriesPoint() {};
    ~TimeSeriesPoint() {};

    int32_t timestamp_;
    double value_;
};

}}//incolun::clothodb

