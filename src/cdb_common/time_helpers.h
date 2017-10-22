#pragma once

#include <cstdint>

namespace cdb{
namespace ts{

class time_helpers
{
public:
    static uint64_t make_timestamp(int year, int month, int day, int hour, int minute, int seconds, int milliseconds = 0);
    static uint64_t timestamp_from_hours(int hours);
    static uint64_t timestamp_from_minutes(int minutes);
    static uint64_t timestamp_from_seconds(int seconds);

    static uint64_t round_down_to_hour(uint64_t timestamp);
};

}}

