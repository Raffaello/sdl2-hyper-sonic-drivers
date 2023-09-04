#pragma once

#include <cstdint>
#include <limits>
#include <bit>
#include <HyperSonicDrivers/audio/scummvm/AudioStream.hpp> // TODO redo it, avoid to use scummvm namespace

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
     * Fixed-point fractions, used by the sound rate converter and other code.
     */
    typedef int32_t frac_t;

    /**
    * The default fractional type in frac.h (with 16 fractional bits) limits
    * the rate conversion code to 65536Hz audio: we need to able to handle
    * 96kHz audio, so we use fewer fractional bits in this code.
    */
    constexpr int fracBitsLow = std::countl_zero<uint16_t>(0) - 1;
    constexpr int fracOneLow = (1L << fracBitsLow);
    constexpr int fracHalfLow = (1L << (fracBitsLow - 1));

    class IRateConverter
    {
    public:
        IRateConverter() = default;
        virtual ~IRateConverter() = default;

        /**
         * @return Number of sample pairs written into the buffer.
         */
        virtual int flow(HyperSonicDrivers::audio::scummvm::AudioStream& input, int16_t* obuf, uint32_t osamp, uint16_t vol_l, uint16_t vol_r) = 0;

        virtual int drain(int16_t* obuf, uint32_t osamp, uint16_t vol) = 0;
    };

    IRateConverter* makeIRateConverter(
        const uint32_t inrate,
        const uint32_t outrate,
        const bool stereo,
        const bool reverseStereo = false
    );
}