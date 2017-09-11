#include "src/TimeSeries/compressor/Decompressor.h"

#include "src/TimeSeries/Constants.h"
#include "src/TimeSeries/BitUtils.h"
#include "src/TimeSeries/BitStream.h"
#include "src/TimeSeries/ErrorCodes.h"

namespace incolun {
namespace clothodb {

TimeStampDecompressor::TimeStampDecompressor(BitStream& stream)
    :m_stream(stream)
{
    m_prevTimestampDelta = 0;
    m_prevTimestamp = 0;
}

uint32_t TimeStampDecompressor::GetFirstValue()
{
    m_prevTimestamp = m_stream.ReadBits32(Constants::kFirstTimestampBits);
    m_prevTimestampDelta = 0;
    return m_prevTimestamp;
}

uint32_t TimeStampDecompressor::GetNextValue()
{
    auto deltaOfDelta = ReadTimestampDeltaOfDelta();

    m_prevTimestampDelta += deltaOfDelta;
    m_prevTimestamp += m_prevTimestampDelta;
    return m_prevTimestamp;
}

__inline int32_t TimeStampDecompressor::ReadTimestampDeltaOfDelta()
{
    if (m_stream.ReadBit() == 0)
    {
        return 0;
    }
    
    uint32_t deltaOfDelta = 0;
    if (m_stream.ReadBit() == 0)
    {
        deltaOfDelta = m_stream.ReadBits32(7);
    }
    else if (m_stream.ReadBit() == 0)
    {
        deltaOfDelta = m_stream.ReadBits32(9);
    }
    else
    {
        deltaOfDelta = m_stream.ReadBits32(13);
    }
    return BitUtils::DecodeZigZag32(++deltaOfDelta);
}

DoubleDecompressor::DoubleDecompressor(BitStream& stream)
    :m_stream(stream)
{
    m_prevValue = 0;
    m_prevValueTZ = 0;
    m_prevValueLZ = 0;
}

uint64_t DoubleDecompressor::GetFirstValue()
{
    return GetNextValue();
}

uint64_t DoubleDecompressor::GetNextValue()
{
    uint64_t xorValue = 0;
    if (m_stream.ReadBit() == 0)
    {
        m_prevValue = m_prevValue ^ xorValue;
    }
    else if (m_stream.ReadBit() == 0)
    {
        m_prevValueLZ = m_stream.ReadBits32(Constants::kDoubleLeadingZerosLengthBits);
        uint32_t blockSize = m_stream.ReadBits32(Constants::kDoubleBlockSizeLengthBits) + 1;
        m_prevValueTZ = 64 - blockSize - m_prevValueLZ;

        xorValue = m_stream.ReadBits64(blockSize);
        xorValue <<= m_prevValueTZ;
    }
    else
    {
        //use information from previous block
        xorValue = m_stream.ReadBits64(64 - m_prevValueTZ - m_prevValueLZ);
        xorValue <<= m_prevValueTZ;
    }

    uint64_t value = xorValue ^ m_prevValue;
    m_prevValue = value;
    return value;
}

IntegerDecompressor::IntegerDecompressor(BitStream& stream)
    :m_stream(stream)
{
    m_prevIntegerDelta = 0;
    m_prevInteger = 0;
}

uint64_t IntegerDecompressor::GetFirstValue()
{
    m_prevInteger = m_stream.ReadBits64(64);
    m_prevIntegerDelta = 0;
    return m_prevInteger;
}

uint64_t IntegerDecompressor::GetNextValue()
{
    auto deltaOfDelta = ReadIntegerDeltaOfDelta();

    m_prevIntegerDelta += deltaOfDelta;
    m_prevInteger += m_prevIntegerDelta;
    return m_prevInteger;
}

__inline int64_t IntegerDecompressor::ReadIntegerDeltaOfDelta()
{
    if (m_stream.ReadBit() == 0)
    {
        return 0;
    }

    uint64_t deltaOfDelta = 0;
    if (m_stream.ReadBit() == 0)
    {
        deltaOfDelta = m_stream.ReadBits32(7);
    }
    else if (m_stream.ReadBit() == 0) 
    {
        deltaOfDelta = m_stream.ReadBits32(9);
    }
    else if (m_stream.ReadBit() == 0)
    {
        deltaOfDelta = m_stream.ReadBits32(16);
    }
    else
    {
        deltaOfDelta = m_stream.ReadBits64(64);
    }
    return BitUtils::DecodeZigZag64(++deltaOfDelta);
}

VectorOrError<uint32_t> Decompressor::DecompressTimeStamps(BitStream& stream)
{
    return Decompress<uint32_t, TimeStampDecompressor>(stream);
}

VectorOrError<uint64_t> Decompressor::DecompressIntegerValues(BitStream& stream)
{
    return Decompress<uint64_t, IntegerDecompressor>(stream);
}

VectorOrError<double> Decompressor::DecompressDoubleValues(BitStream& stream)
{
    return Decompress<double, DoubleDecompressor>(stream);
}

template<typename T, class D>
VectorOrError<T> Decompressor::Decompress(BitStream& stream)
{
    stream.SetPosition(0);
    
    std::shared_ptr<std::vector<T>> result;
    if (stream.IsEmpty()) return VectorOrError<T>::fromValue(result);

    try
    {
        D decompressor(stream);
        auto firstValue = decompressor.GetFirstValue();
        result->push_back(*((T*)&firstValue));

        while (stream.CanRead())
        {
            auto nextValue = decompressor.GetNextValue();
            result->push_back(*((T*)&nextValue));
        }

        return VectorOrError<T>::fromValue(result);
    }
    catch (const std::overflow_error&)
    {
        return VectorOrError<T>::fromError(ErrorCodes::kBitStreamOverflaw);
    }
}

}}