#include "time_helpers.h"
#include "src/cdb_common/constants.h"

#include <time.h>

namespace cdb{
namespace ts{

uint64_t time_helpers::make_timestamp(int year, int month, int day, int hour, int minute, int seconds, int milliseconds)
{
    tm t = {};
    t.tm_year = year;
    t.tm_mon = month;
    t.tm_mday = day;
    t.tm_hour = hour;
    t.tm_min = minute;
    t.tm_sec = seconds;
    auto time_in_ms = mktime(&t);
    time_in_ms += milliseconds;
    return time_in_ms;
}

uint64_t time_helpers::timestamp_from_hours(int hours)
{
    return hours * timestamp_from_minutes(60);
}

uint64_t time_helpers::timestamp_from_minutes(int minutes)
{
    return minutes * timestamp_from_seconds(60);
}

uint64_t time_helpers::timestamp_from_seconds(int seconds)
{
    return Constants::kMillisecondsInSec * seconds;
}

uint64_t time_helpers::round_down_to_hour(uint64_t timestamp)
{
    return (timestamp / Constants::kOneHourInMs) * Constants::kOneHourInMs;
}

}}