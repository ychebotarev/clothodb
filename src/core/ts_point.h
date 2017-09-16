#pragma once

#include <cstdint>

namespace clothodb {
namespace core {

struct ts_point
{
    uint64_t value;
    uint64_t timestamp;
};

}}