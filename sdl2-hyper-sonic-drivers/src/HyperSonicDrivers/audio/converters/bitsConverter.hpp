#pragma

#include <cstdint>
#include <span>

namespace HyperSonicDrivers::audio::converters
{
    int16_t* convert8to16(const uint8_t samples[], const size_t size);
    std::span<int16_t> convert8to16(const std::span<uint8_t> samples);
}
