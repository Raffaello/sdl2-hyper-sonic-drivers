#pragma once

#include <audio/scummvm/AudioStream.hpp>
#include <cstdint>

namespace audio
{
    namespace scummvm
    {
        // TODO remove these typedefs
        typedef int16_t st_sample_t;
        typedef uint16_t st_volume_t;
        typedef uint32_t st_size_t;
        typedef uint32_t st_rate_t;

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
            virtual int flow(AudioStream& input, st_sample_t* obuf, st_size_t osamp, st_volume_t vol_l, st_volume_t vol_r) = 0;

            virtual int drain(st_sample_t* obuf, st_size_t osamp, st_volume_t vol) = 0;
        };

        RateConverter* makeRateConverter(st_rate_t inrate, st_rate_t outrate, bool stereo, bool reverseStereo = false);
    }
}
