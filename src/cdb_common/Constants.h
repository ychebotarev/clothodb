#pragma once

#include <cstdint>

namespace cdb {

class Constants
{
public:
    static constexpr uint32_t kFirstTimestampBits = 13;

    static constexpr uint32_t kDoubleLeadingZerosLengthBits = 5;
    static constexpr uint32_t kDoubleBlockSizeLengthBits = 6;
    static constexpr uint32_t kMillisecondsBits = 10;

    static constexpr uint32_t kDoubleMaxLeadingZerosLength = (1 << kDoubleLeadingZerosLengthBits) - 1;
    static constexpr uint32_t kOneHourInSeconds = 60 * 60;
    static constexpr uint32_t kMillisecondsInSec = 1000;
    static constexpr uint32_t kOneHourInMs = kOneHourInSeconds * kMillisecondsInSec;
    
    static constexpr uint32_t kBucketSize = 60 * 60;
    static constexpr size_t kMaxBuckets = 25;
};

}