#pragma once

#include <cstdint>
#include <limits>
#include <bit>
#include <memory>
#include <algorithm>
#include <HyperSonicDrivers/audio/IAudioStream.hpp>
#include <HyperSonicDrivers/audio/mixer/ChannelGroup.hpp>

namespace HyperSonicDrivers::audio::converters
{
    /**
    * The size of the intermediate input cache. Bigger values may increase
    * performance, but only until some point (depends largely on cache size,
    * target processor and various other factors), at which it will decrease
    * again.
    */
    constexpr int intermediateBufferSize = 512;

    /**
    * The default fractional type in frac.h (with 16 fractional bits) limits
    * the rate conversion code to 65536Hz audio: we need to able to handle
    * 96kHz audio, so we use fewer fractional bits in this code.
    */
    constexpr int fracBitsLow = std::countl_zero<uint16_t>(0) - 1;
    constexpr int fracOneLow = (1L << fracBitsLow);
    constexpr int fracHalfLow = (1L << (fracBitsLow - 1));

    constexpr void clampAdd(int16_t& a, int b)
    {
        a = static_cast<int16_t>(std::clamp<int32_t>(
                a + b,
                std::numeric_limits<int16_t>::min(),
                std::numeric_limits<int16_t>::max()
        ));
    }

    constexpr void output_channel(int16_t& out_buf, const int16_t out, const uint16_t vol)
    {
        clampAdd(out_buf, (out * static_cast<int>(vol)) / mixer::Mixer_max_volume);
    }

    class IRateConverter
    {
    public:
        IRateConverter() = default;
        virtual ~IRateConverter() = default;

        /**
         * @return Number of sample pairs written into the buffer.
         */
        virtual size_t flow(IAudioStream& input, int16_t* obuf, uint32_t osamp, uint16_t vol_l, uint16_t vol_r) = 0;
        virtual size_t drain(int16_t* obuf, uint32_t osamp, uint16_t vol) = 0;
    };

    std::unique_ptr<IRateConverter> makeIRateConverter(
        const uint32_t inrate,
        const uint32_t outrate,
        const bool stereo,
        const bool reverseStereo = false
    );
}
