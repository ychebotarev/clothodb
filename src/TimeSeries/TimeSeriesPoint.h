#pragma once

#include <cstdint>

namespace incolun {
namespace clothodb {

struct TimeSeriesPoint
{
    uint64_t timestamp;
    uint64_t value;
};

}}//incolun::clothodb