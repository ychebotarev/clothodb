#pragma once

#include <cstdint>
#include "ts_scale.h"

namespace cdb{
namespace core{

class time_helpers
{
public:
    static uint64_t scale_timestamp(uint64_t timestamp, ts_scale scale);
    static uint32_t scale_in_ms(ts_scale scale);
    static uint64_t make_timestamp(int year, int month, int day, int hour, int minute, int seconds, int milliseconds = 0);
    static uint64_t timestamp_from_hours(int hours);
    static uint64_t timestamp_from_minutes(int minutes);
    static uint64_t timestamp_from_seconds(int seconds);

    static uint64_t floor_to_hour(uint64_t timestamp);
};

}}

