#pragma

#include <cstdint>

namespace HyperSonicDrivers::audio::converters
{
    int16_t* convert8to16(const uint8_t samples[], const size_t size);
}
