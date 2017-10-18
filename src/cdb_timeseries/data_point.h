#pragma once

#include <cstdint>

namespace cdb{
namespace ts{

struct data_point
{
    uint64_t value;
    uint64_t timestamp;
};

}}