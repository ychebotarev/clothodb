#pragma once

#include <cstdint>

namespace cdb{
namespace core{

struct ts_point
{
    uint64_t value;
    uint64_t timestamp;
};

}}