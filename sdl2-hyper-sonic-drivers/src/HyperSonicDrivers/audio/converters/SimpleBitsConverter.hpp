#pragma

#include <cstdint>
#include <vector>

namespace HyperSonicDrivers::audio::converters
{
    class SimpleBitsConverter
    {
    public:
        SimpleBitsConverter() = default;
        ~SimpleBitsConverter() = default;

        //size_t flow(int16_t out_buf, uint32_t out_samples, uint16_t vol);

        //const uint8_t in_bits;
        //const uint8_t out_bits;
 
        static int16_t* convert8to16(const uint8_t samples[], const size_t size);
    };
}
