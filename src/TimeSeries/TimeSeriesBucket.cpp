#include "src/TimeSeries/TimeSeriesBucket.h"
#include "src/TimeSeries/Compressor/Decompressor.h"

namespace incolun{
namespace clothodb{

TimeSeriesBucket::TimeSeriesBucket(const TimeSeriesConfig& config):
    m_config(config),
    m_Sealed(false)
{
    m_stream = std::make_shared<BitStream>();
    m_timeStampCompressor = std::make_unique<TimestampCompressor>(*m_stream.get());
    switch (m_config.m_type)
    {
    case TimeSeriesType::TypeDouble:
    {
        m_valueCompressor = std::make_unique<DoubleCompressor>(*m_stream.get());
    }
    case TimeSeriesType::TypeInteger:
    {
        m_valueCompressor = std::make_unique<IntegerCompressor>(*m_stream.get());
    }
    default:
        break;
    }
}

TimeSeriesBucket::~TimeSeriesBucket()
{
}

void TimeSeriesBucket::Reset()
{
    m_stream->SetLength(0);
    m_stream->SetPosition(0);
    m_Sealed = false;
}

void TimeSeriesBucket::AddValue(uint64_t value, uint32_t timestamp)
{
    uint32_t milliseconds = 0;
    if (m_config.m_storeMilliseconds)
    {
        milliseconds = timestamp % 1000;
    }
    timestamp /= 1000;

    bool firstValue = m_stream->IsEmpty();

    if (firstValue)
    {
        m_timeStampCompressor->AppendFirstValue(timestamp);
        m_valueCompressor->AppendFirstValue(&value);
    }
    else
    {
        m_timeStampCompressor->AppendNextValue(timestamp);
        m_valueCompressor->AppendNextValue(&value);
    }

    if (m_config.m_storeMilliseconds)
    {
        m_stream->WriteBits32(milliseconds, 10);
    }
}

void TimeSeriesBucket::Decompress(
    std::vector<TimeSeriesPoint>& points, 
    uint64_t baseTime,
    uint64_t startTime, 
    uint64_t endTime)
{
    std::unique_ptr<ValueDecompressor> decompressor;
    if(m_config.m_type == TimeSeriesType::TypeDouble)
    {
        decompressor = std::unique_ptr<DoubleDecompressor>(new DoubleDecompressor(*m_stream.get()));
    }
    if (m_config.m_type == TimeSeriesType::TypeInteger)
    {
        decompressor = std::unique_ptr<IntegerDecompressor>(new IntegerDecompressor(*m_stream.get()));
    }
    
    m_stream->SetPosition(0);
    
    TimeStampDecompressor timeStampDecompressor(*m_stream.get());
    uint64_t timestamp = timeStampDecompressor.GetFirstValue();
    timestamp += baseTime;
    timestamp *= 1000;

    if (m_config.m_storeMilliseconds)
    {
        uint32_t milliseconds = m_stream->ReadBits32(10);
        timestamp += milliseconds;
    }
    
    uint64_t value = decompressor->GetFirstValue();
    if (timestamp >= startTime && timestamp <= endTime)
        points.push_back({ timestamp, value });

    while (m_stream->CanRead())
    {
        timestamp = timeStampDecompressor.GetNextValue();
        timestamp += baseTime;
        timestamp *= 1000;
        if (m_config.m_storeMilliseconds)
        {
            uint32_t milliseconds = m_stream->ReadBits32(10);
            timestamp += milliseconds;
        }
        value = decompressor->GetNextValue();
        if (timestamp >= startTime && timestamp <= endTime)
            points.push_back({ timestamp, value });
    }
}

void TimeSeriesBucket::Seal()
{
    m_stream->Seal();
    m_Sealed = true;
}

}}
