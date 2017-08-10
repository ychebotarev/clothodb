#pragma once

#include <stdint.h>

class BitStreamConstants
{
public:
    static constexpr uint32_t kFirstTimestampBits = 13;
    static constexpr uint32_t kDefaultDelta = 60;

    static constexpr uint32_t kLeadingZerosLengthBits = 5;
    static constexpr uint32_t kBlockSizeLengthBits = 6;
    static constexpr uint32_t kBlockExtraSizeLengthBits = kLeadingZerosLengthBits + kBlockSizeLengthBits;

    static constexpr uint32_t kMaxLeadingZerosLength = (1 << kLeadingZerosLengthBits) - 1;
};