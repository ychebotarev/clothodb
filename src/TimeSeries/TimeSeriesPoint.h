#pragma once

#include <cstdint>

namespace incolun {
namespace clothodb {

struct TimeSeriesPoint
{
    uint64_t value;
    uint64_t timestamp;
};

}}//incolun::clothodb