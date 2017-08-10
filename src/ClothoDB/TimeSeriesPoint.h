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
    int64_t value_;
};

}}//incolun::clothodb

