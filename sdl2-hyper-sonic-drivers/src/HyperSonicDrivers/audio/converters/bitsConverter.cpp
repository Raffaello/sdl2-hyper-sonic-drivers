#include <HyperSonicDrivers/audio/converters/bitsConverter.hpp>

namespace HyperSonicDrivers::audio::converters
{
    int16_t* convert8to16(const uint8_t samples[], const size_t size)
    {
        int16_t* res = new int16_t[size];

        for (size_t i = 0; i < size; i++)
            res[i] = static_cast<int16_t>((samples[i] - 128) << 8);

        return res;
    }

    std::span<int16_t> convert8to16(const std::span<uint8_t> samples)
    {
        return std::span<int16_t>(convert8to16(samples.data(), samples.size()), samples.size());
    }
}
