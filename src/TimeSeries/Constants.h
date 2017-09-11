#pragma once

#include <cstdint>

namespace incolun {
namespace clothodb {

class Constants
{
public:
    static constexpr uint32_t kFirstTimestampBits = 13;

    static constexpr uint32_t kDoubleLeadingZerosLengthBits = 5;
    static constexpr uint32_t kDoubleBlockSizeLengthBits = 6;

    static constexpr uint32_t kDoubleMaxLeadingZerosLength = (1 << kDoubleLeadingZerosLengthBits) - 1;

    static constexpr uint32_t kOneHourInSeconds = 60 * 60;
    static constexpr uint32_t kOneHourInMs = kOneHourInSeconds * 1000;
};

}}