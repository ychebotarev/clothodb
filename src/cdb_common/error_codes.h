#pragma once

#include <cstdint>

namespace cdb {

class ErrorCodes
{
public:
    static constexpr uint32_t kBitStreamUnknown = 1;
    static constexpr uint32_t kBitStreamOverflaw = 2;
    static constexpr uint32_t kBitStreamCantRead = 3;
    static constexpr uint32_t kFailedToParseTags = 4;
    static constexpr uint32_t kNotImplemented = 5;
    static constexpr uint32_t kTimeSeriesAlreadyExist = 6;
    static constexpr uint32_t kLookupMetricAlreadyExist = 7;
    static constexpr uint32_t kErrorCreatingGuid = 8;
    static constexpr uint32_t kTimeSeriesNotFound = 9;
    static constexpr uint32_t kHashCollision = 10;
    static constexpr uint32_t kLookupCollision = 11;
    static constexpr uint32_t kNamespaceAlreadyExist = 12;
    static constexpr uint32_t kNamespaceNotExist = 13;
    static constexpr uint32_t kInvalidArguments = 14;
    static constexpr uint32_t kLookupMetricNotFound = 15;
};

}