#pragma once

#include <cstdint>
#include <HyperSonicDrivers/audio/scummvm/AudioStream.hpp>

namespace HyperSonicDrivers::audio::scummvm
{
/**
* The size of the intermediate input cache. Bigger values may increase
* performance, but only until some point (depends largely on cache size,
* target processor and various other factors), at which it will decrease
* again.
*/
constexpr int INTERMEDIATE_BUFFER_SIZE = 512;

/**
     * Fixed-point fractions, used by the sound rate converter and other code.
     */
typedef int32_t frac_t;


/**
 * The precision of the fractional (fixed-point) type that is defined below.
 * Normally, you should never need to modify this value.
 */
enum {
    FRAC_BITS = 16,
    FRAC_LO_MASK = ((1L << FRAC_BITS) - 1),
    FRAC_HI_MASK = ((1L << FRAC_BITS) - 1) << FRAC_BITS,

    FRAC_ONE = (1L << FRAC_BITS),		// 1.0
    FRAC_HALF = (1L << (FRAC_BITS - 1))	// 0.5
};

/**
* The default fractional type in frac.h (with 16 fractional bits) limits
* the rate conversion code to 65536Hz audio: we need to able to handle
* 96kHz audio, so we use fewer fractional bits in this code.
*/
    enum {
        FRAC_BITS_LOW = 15,
        FRAC_ONE_LOW = (1L << FRAC_BITS_LOW),
        FRAC_HALF_LOW = (1L << (FRAC_BITS_LOW - 1))
    };

    /* Minimum and maximum values a sample can hold. */
    enum {
        ST_SAMPLE_MAX = 0x7fffL,
        ST_SAMPLE_MIN = (-ST_SAMPLE_MAX - 1L)
    };

    enum {
        ST_EOF = -1,
        ST_SUCCESS = 0
    };

    static inline void clampedAdd(int16_t& a, int b) {
        int val;
#ifdef OUTPUT_UNSIGNED_AUDIO
        val = (a ^ 0x8000) + b;
#else
        val = a + b;
#endif

        if (val > ST_SAMPLE_MAX)
            val = ST_SAMPLE_MAX;
        else if (val < ST_SAMPLE_MIN)
            val = ST_SAMPLE_MIN;

#ifdef OUTPUT_UNSIGNED_AUDIO
        a = ((int16)val) ^ 0x8000;
#else
        a = val;
#endif
    }

    class RateConverter
    {
    public:
        RateConverter() {}
        virtual ~RateConverter() {}

        /**
         * @return Number of sample pairs written into the buffer.
         */
        virtual int flow(AudioStream& input, int16_t* obuf, uint32_t osamp, uint16_t vol_l, uint16_t vol_r) = 0;

        virtual int drain(int16_t* obuf, uint32_t osamp, uint16_t vol) = 0;
    };

    RateConverter* makeRateConverter(uint32_t inrate, uint32_t outrate, bool stereo, bool reverseStereo = false);
}
