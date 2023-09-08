#include <HyperSonicDrivers/audio/converters/SimpleBitsConverter.hpp>

namespace HyperSonicDrivers::audio::converters
{
    int16_t* SimpleBitsConverter::convert8to16(const uint8_t samples[], const size_t size)
    {
        int16_t* res = new int16_t[size];

        for (size_t i = 0; i < size; i++)
            res[i] = (samples[i] - 128) << 8;

        //size *= (sizeof(int16_t)/sizeof(uint8_t));
        return res;
    }
}
