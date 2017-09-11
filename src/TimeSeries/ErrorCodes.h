#pragma once

#include <cstdint>

namespace incolun {
namespace clothodb {

class ErrorCodes
{
public:
    static constexpr uint32_t kBitStreamUnknown = 1;
    static constexpr uint32_t kBitStreamOverflaw = 2;
    static constexpr uint32_t kBitStreamCantRead = 3;
};

}}