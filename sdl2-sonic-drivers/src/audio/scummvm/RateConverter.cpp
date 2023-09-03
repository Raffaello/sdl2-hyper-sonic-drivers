#include <audio/scummvm/RateConverter.hpp>
#include <audio/scummvm/AudioStream.hpp>
#include <audio/scummvm/Mixer.hpp>
#include <cassert>
#include <cstdlib>
#include <SDL2/SDL_log.h>
#include <format>

namespace HyperSonicDrivers::audio::scummvm
{
    // TODO split into multiple class and in its own namesapce

    // TODO: move to utils and as a constexpr
#define ARRAYSIZE(x) ((int)(sizeof(x) / sizeof(x[0])))

        // frac.h -----------------------------------------------------------------
     /**
      * @defgroup common_frac Fixed-point fractions
      * @ingroup common
      *
      * @brief  API for fixed-point fractions.
      *
      * @{
      */

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
     * Fixed-point fractions, used by the sound rate converter and other code.
     */
    typedef int32_t frac_t;

    inline frac_t doubleToFrac(double value) { return (frac_t)(value * static_cast<int>(FRAC_ONE)); }
    inline double fracToDouble(frac_t value) { return ((double)value) / static_cast<int>(FRAC_ONE); }

    inline frac_t intToFrac(int16_t value) { return value * (1 << FRAC_BITS); }
    inline int16_t fracToInt(frac_t value) { return value / (1 << FRAC_BITS); }

    /** @} */

    // ------------------------------------------------------------------------

    /*
    * The code in this file is based on code with Copyright 1998 Fabrice Bellard
    * Fabrice original code is part of SoX (http://sox.sourceforge.net).
    * Max Horn adapted that code to the needs of ScummVM and rewrote it partial,
    * in the process removing any use of floating point arithmetic. Various other
    * improvements over the original code were made.
    */


    /**
    * The size of the intermediate input cache. Bigger values may increase
    * performance, but only until some point (depends largely on cache size,
    * target processor and various other factors), at which it will decrease
    * again.
    */
#define INTERMEDIATE_BUFFER_SIZE 512

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

    /**
     * Audio rate converter based on simple resampling. Used when no
     * interpolation is required.
     *
     * Limited to sampling frequency <= 65535 Hz.
     */
    template<bool stereo, bool reverseStereo>
    class SimpleRateConverter : public RateConverter {
    protected:
        int16_t inBuf[INTERMEDIATE_BUFFER_SIZE] = {};
        const int16_t* inPtr = nullptr;
        int inLen = 0;

        /** position of how far output is ahead of input */
        /** Holds what would have been opos-ipos */
        long opos = 0;

        /** fractional position increment in the output stream */
        long opos_inc = 0;

    public:
        SimpleRateConverter(uint32_t inrate, uint32_t outrate);
        int flow(AudioStream& input, int16_t* obuf, uint32_t osamp, uint16_t vol_l, uint16_t vol_r);
        int drain(int16_t* obuf, uint32_t osamp, uint16_t vol) {
            return ST_SUCCESS;
        }
    };


    /*
     * Prepare processing.
     */
    template<bool stereo, bool reverseStereo>
    SimpleRateConverter<stereo, reverseStereo>::SimpleRateConverter(uint32_t inrate, uint32_t outrate) {
        if ((inrate % outrate) != 0) {
            SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Input rate must be a multiple of output rate to use rate effect");
        }

        if (inrate >= 65536 || outrate >= 65536) {
            SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "rate effect can only handle rates < 65536");
        }

        opos = 1;

        /* increment */
        opos_inc = inrate / outrate;

        inLen = 0;
    }

    /*
     * Processed signed long samples from ibuf to obuf.
     * Return number of sample pairs processed.
     */
    template<bool stereo, bool reverseStereo>
    int SimpleRateConverter<stereo, reverseStereo>::flow(AudioStream& input, int16_t* obuf, uint32_t osamp, uint16_t vol_l, uint16_t vol_r) {
        int16_t* ostart, * oend;

        ostart = obuf;
        oend = obuf + osamp * 2;

        while (obuf < oend) {

            // read enough input samples so that opos >= 0
            do {
                // Check if we have to refill the buffer
                if (inLen == 0) {
                    inPtr = inBuf;
                    inLen = input.readBuffer(inBuf, ARRAYSIZE(inBuf));
                    if (inLen <= 0)
                        return (obuf - ostart) / 2;
                }
                inLen -= (stereo ? 2 : 1);
                opos--;
                if (opos >= 0) {
                    inPtr += (stereo ? 2 : 1);
                }
            } while (opos >= 0);

            int16_t out0, out1;
            out0 = *inPtr++;
            out1 = (stereo ? *inPtr++ : out0);

            // Increment output position
            opos += opos_inc;

            // output left channel
            clampedAdd(obuf[reverseStereo], (out0 * (int)vol_l) / Mixer::MaxVolume::MIXER);

            // output right channel
            clampedAdd(obuf[reverseStereo ^ 1], (out1 * (int)vol_r) / Mixer::MaxVolume::MIXER);

            obuf += 2;
        }
        return (obuf - ostart) / 2;
    }

    /**
     * Audio rate converter based on simple linear Interpolation.
     *
     * The use of fractional increment allows us to use no buffer. It
     * avoid the problems at the end of the buffer we had with the old
     * method which stored a possibly big buffer of size
     * lcm(in_rate,out_rate).
     *
     * Limited to sampling frequency <= 65535 Hz.
     */

    template<bool stereo, bool reverseStereo>
    class LinearRateConverter : public RateConverter {
    protected:
        int16_t inBuf[INTERMEDIATE_BUFFER_SIZE] = {};
        const int16_t* inPtr = nullptr;
        int inLen = 0;

        /** fractional position of the output stream in input stream unit */
        frac_t opos = 0;

        /** fractional position increment in the output stream */
        frac_t opos_inc = 0;

        /** last sample(s) in the input stream (left/right channel) */
        int16_t ilast0 = 0;
        int16_t ilast1 = 0;
        /** current sample(s) in the input stream (left/right channel) */
        int16_t icur0 = 0;
        int16_t icur1 = 0;

    public:
        LinearRateConverter(uint32_t inrate, uint32_t outrate);
        int flow(AudioStream& input, int16_t* obuf, uint32_t osamp, uint16_t vol_l, uint16_t vol_r);
        int drain(int16_t* obuf, uint32_t osamp, uint16_t vol) {
            return ST_SUCCESS;
        }
    };


    /*
     * Prepare processing.
     */
    template<bool stereo, bool reverseStereo>
    LinearRateConverter<stereo, reverseStereo>::LinearRateConverter(uint32_t inrate, uint32_t outrate) {
        if (inrate >= 131072 || outrate >= 131072) {
            SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "rate effect can only handle rates < 131072");
        }

        opos = FRAC_ONE_LOW;

        // Compute the linear interpolation increment.
        // This will overflow if inrate >= 2^17, and underflow if outrate >= 2^17.
        // Also, if the quotient of the two rate becomes too small / too big, that
        // would cause problems, but since we rarely scale from 1 to 65536 Hz or vice
        // versa, I think we can live with that limitation ;-).
        opos_inc = (inrate << FRAC_BITS_LOW) / outrate;

        ilast0 = ilast1 = 0;
        icur0 = icur1 = 0;

        inLen = 0;
    }

    /*
     * Processed signed long samples from ibuf to obuf.
     * Return number of sample pairs processed.
     */
    template<bool stereo, bool reverseStereo>
    int LinearRateConverter<stereo, reverseStereo>::flow(AudioStream& input, int16_t* obuf, uint32_t osamp, uint16_t vol_l, uint16_t vol_r) {
        int16_t* ostart, * oend;

        ostart = obuf;
        oend = obuf + osamp * 2;

        while (obuf < oend) {

            // read enough input samples so that opos < 0
            while ((frac_t)FRAC_ONE_LOW <= opos) {
                // Check if we have to refill the buffer
                if (inLen == 0) {
                    inPtr = inBuf;
                    inLen = input.readBuffer(inBuf, ARRAYSIZE(inBuf));
                    if (inLen <= 0)
                        return (obuf - ostart) / 2;
                }
                inLen -= (stereo ? 2 : 1);
                ilast0 = icur0;
                icur0 = *inPtr++;
                if (stereo) {
                    ilast1 = icur1;
                    icur1 = *inPtr++;
                }
                opos -= FRAC_ONE_LOW;
            }

            // Loop as long as the outpos trails behind, and as long as there is
            // still space in the output buffer.
            while (opos < (frac_t)FRAC_ONE_LOW && obuf < oend) {
                // interpolate
                int16_t out0, out1;
                out0 = (int16_t)(ilast0 + (((icur0 - ilast0) * opos + FRAC_HALF_LOW) >> FRAC_BITS_LOW));
                out1 = (stereo ?
                    (int16_t)(ilast1 + (((icur1 - ilast1) * opos + FRAC_HALF_LOW) >> FRAC_BITS_LOW)) :
                    out0);

                // output left channel
                clampedAdd(obuf[reverseStereo], (out0 * (int)vol_l) / Mixer::MaxVolume::MIXER);

                // output right channel
                clampedAdd(obuf[reverseStereo ^ 1], (out1 * (int)vol_r) / Mixer::MaxVolume::MIXER);

                obuf += 2;

                // Increment output position
                opos += opos_inc;
            }
        }
        return (obuf - ostart) / 2;
    }

    /**
     * Simple audio rate converter for the case that the inrate equals the outrate.
     */
    template<bool stereo, bool reverseStereo>
    class CopyRateConverter : public RateConverter {
        int16_t* _buffer;
        uint32_t _bufferSize;
    public:
        CopyRateConverter() : _buffer(0), _bufferSize(0) {}
        ~CopyRateConverter() {
            free(_buffer);
        }

        virtual int flow(AudioStream& input, int16_t* obuf, uint32_t osamp, uint16_t vol_l, uint16_t vol_r) {
            assert(input.isStereo() == stereo);

            int16_t* ptr;
            uint32_t len;

            int16_t* ostart = obuf;

            if (stereo)
                osamp *= 2;

            // Reallocate temp buffer, if necessary
            if (osamp > _bufferSize) {
                free(_buffer);
                _buffer = (int16_t*)malloc(osamp * 2);
                _bufferSize = osamp;
            }

            if (_buffer == nullptr) {
                SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "[CopyRateConverter::flow] Cannot allocate memory for temp buffer");
                return 0;
            }

            // Read up to 'osamp' samples into our temporary buffer
            len = input.readBuffer(_buffer, osamp);

            // Mix the data into the output buffer
            ptr = _buffer;
            for (; len > 0; len -= (stereo ? 2 : 1)) {
                int16_t out0, out1;
                out0 = *ptr++;
                out1 = (stereo ? *ptr++ : out0);

                // output left channel
                clampedAdd(obuf[reverseStereo], (out0 * (int)vol_l) / Mixer::MaxVolume::MIXER);

                // output right channel
                clampedAdd(obuf[reverseStereo ^ 1], (out1 * (int)vol_r) / Mixer::MaxVolume::MIXER);

                obuf += 2;
            }
            return (obuf - ostart) / 2;
        }

        virtual int drain(int16_t* obuf, uint32_t osamp, uint16_t vol) {
            return ST_SUCCESS;
        }
    };


    template<bool stereo, bool reverseStereo>
    RateConverter* makeRateConverter(uint32_t inrate, uint32_t outrate) {
        if (inrate != outrate) {
            if ((inrate % outrate) == 0 && (inrate < 65536)) {
                return new SimpleRateConverter<stereo, reverseStereo>(inrate, outrate);
            }
            else {
                return new LinearRateConverter<stereo, reverseStereo>(inrate, outrate);
            }
        }
        else {
            return new CopyRateConverter<stereo, reverseStereo>();
        }
    }

    /**
     * Create and return a RateConverter object for the specified input and output rates.
     */
    RateConverter* makeRateConverter(uint32_t inrate, uint32_t outrate, bool stereo, bool reverseStereo) {
        if (stereo) {
            if (reverseStereo)
                return makeRateConverter<true, true>(inrate, outrate);
            else
                return makeRateConverter<true, false>(inrate, outrate);
        }
        else
            return makeRateConverter<false, false>(inrate, outrate);
    }
}
