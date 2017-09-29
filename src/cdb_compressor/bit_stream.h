#pragma once

#include <memory>
#include <vector>
#include <stdexcept>

namespace cdb{
namespace compressor{

//class is NOT thread safe
class bit_stream
{
private:
    std::vector<uint8_t> m_buffer;
    uint32_t m_commited_bits;

public:
    bit_stream();
    bit_stream(size_t capacity);

    void write_bit(uint32_t bit_position, uint32_t value);
    void write_bits32(uint32_t bit_position, uint32_t value, uint8_t bits_to_store);
    void write_bits64(uint32_t bit_position, uint64_t value, uint8_t bits_to_store);

    uint32_t read_bit(uint32_t bit_position) const;
    uint32_t read_bits32(uint32_t bit_position, uint8_t bits_to_read) const;
    uint64_t read_bits64(uint32_t bit_position, uint8_t bits_to_read) const;

    uint8_t& operator[](std::size_t idx)
    { 
        return m_buffer[idx];
    }

    bool is_empty() const { return m_commited_bits == 0; }
    void commit(uint32_t commited_bits) { m_commited_bits += commited_bits; }
	uint32_t get_commited_bits() const { return m_commited_bits; }
    void set_commited_bits(uint32_t commited_bits) { m_commited_bits = commited_bits; }

private:
    inline void set_capacity_bits(uint32_t capacity);
    __forceinline void read_check(uint32_t bit_position) const;
};

class bit_stream_reader
{
public:
    bit_stream_reader(const bit_stream& stream) : m_stream(stream)
    {}
    uint32_t read_bit();
    uint32_t read_bits32(uint8_t bit_to_read);
    uint64_t read_bits64(uint8_t bit_to_read);

    void set_position(uint32_t bit_position) { m_bit_position = bit_position; }
    uint32_t get_position() const { return m_bit_position; }
    bool can_read() const { return m_bit_position < m_stream.get_commited_bits(); }
private:
    const bit_stream& m_stream;
    uint32_t m_bit_position = 0;
};

class bit_stream_writer
{
public:
    bit_stream_writer(bit_stream& stream) : m_stream(stream)
    {}

    void write_bit(uint32_t value);
    void write_bits32(uint32_t value, uint8_t bits_to_store);
    void write_bits64(uint64_t value, uint8_t bits_to_store);

    void set_position(uint32_t bits_position) { m_bit_position = bits_position; }
    uint32_t get_position() const { return m_bit_position; }

    void commit() { m_stream.set_commited_bits(m_bit_position); }
private:
    bit_stream& m_stream;
    uint32_t m_bit_position = 0;
};

}}