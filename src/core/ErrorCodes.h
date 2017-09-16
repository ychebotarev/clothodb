#pragma once

#include <cstdint>

namespace clothodb {
namespace core {

class ErrorCodes
{
public:
    static constexpr uint32_t kBitStreamUnknown = 1;
    static constexpr uint32_t kBitStreamOverflaw = 2;
    static constexpr uint32_t kBitStreamCantRead = 3;
};

}}