#pragma once

#include <stdint.h>
#include <limits>
#include <vector>
#include <string>
#include <memory>

namespace incolun{
namespace clothodb{

class TimeSeriesPoint;
class BitStream;

class BitStreamDecompressor
{
public:
    static std::vector<TimeSeriesPoint> Decompress(const std::shared_ptr<BitStream>& stream);
    static std::vector<uint32_t> DecompressTimeStampts(const std::shared_ptr<BitStream>& stream);
};

}}