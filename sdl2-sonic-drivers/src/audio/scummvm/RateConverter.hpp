#pragma once

#include <audio/scummvm/AudioStream.hpp>
#include <cstdint>

namespace HyperSonicDrivers::audio::scummvm
{
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
