#pragma once

#include <cstdint>
#include <array>
#include <HyperSonicDrivers/audio/converters/IRateConverter.hpp>
#include <HyperSonicDrivers/audio/scummvm/AudioStream.hpp>
#include <HyperSonicDrivers/utils/algorithms.hpp>
#include <HyperSonicDrivers/audio/scummvm/Mixer.hpp>

#include <SDL2/SDL_log.h>

namespace HyperSonicDrivers::audio::converters
{
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
    class LinearRateConverter : public IRateConverter
    {
    protected:
        std::array<int16_t, intermediateBufferSize> inBuf = {};
        const int16_t* inPtr = nullptr;
        int inLen = 0;

        /** fractional position of the output stream in input stream unit */
        int32_t opos = 0;

        /** fractional position increment in the output stream */
        int32_t opos_inc = 0;

        /** last sample(s) in the input stream (left/right channel) */
        int16_t ilast0 = 0;
        int16_t ilast1 = 0;
        /** current sample(s) in the input stream (left/right channel) */
        int16_t icur0 = 0;
        int16_t icur1 = 0;

    public:
        LinearRateConverter(uint32_t inrate, uint32_t outrate);
        int flow(scummvm::AudioStream& input, int16_t* obuf, uint32_t osamp, const uint16_t vol_l, const uint16_t vol_r) override;
        
        int drain(int16_t* obuf, uint32_t osamp, const uint16_t vol) override
        {
            return 0;
        }
    };


    /*
     * Prepare processing.
     */
    template<bool stereo, bool reverseStereo>
    LinearRateConverter<stereo, reverseStereo>::LinearRateConverter(uint32_t inrate, uint32_t outrate)
    {
        if (inrate >= 131072 || outrate >= 131072)
        {
            SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "rate effect can only handle rates < 131072");
        }

        opos = fracOneLow;

        // Compute the linear interpolation increment.
        // This will overflow if inrate >= 2^17, and underflow if outrate >= 2^17.
        // Also, if the quotient of the two rate becomes too small / too big, that
        // would cause problems, but since we rarely scale from 1 to 65536 Hz or vice
        // versa, I think we can live with that limitation ;-).
        opos_inc = (inrate << fracBitsLow) / outrate;

        ilast0 = ilast1 = 0;
        icur0 = icur1 = 0;

        inLen = 0;
    }

    /*
     * Processed signed long samples from ibuf to obuf.
     * Return number of sample pairs processed.
     */
    template<bool stereo, bool reverseStereo>
    int LinearRateConverter<stereo, reverseStereo>::flow(scummvm::AudioStream& input, int16_t* obuf, uint32_t osamp, const uint16_t vol_l, const uint16_t vol_r)
    {
        int16_t* ostart, * oend;

        ostart = obuf;
        oend = obuf + osamp * 2;

        while (obuf < oend)
        {
            // read enough input samples so that opos < 0
            while (fracOneLow <= opos)
            {
                // Check if we have to refill the buffer
                if (inLen == 0)
                {
                    inPtr = inBuf.data();
                    inLen = input.readBuffer(inBuf.data(), inBuf.size());
                    if (inLen <= 0)
                        return (obuf - ostart) / 2;
                }
                inLen -= (stereo ? 2 : 1);
                ilast0 = icur0;
                icur0 = *inPtr++;
                if (stereo)
                {
                    ilast1 = icur1;
                    icur1 = *inPtr++;
                }
                opos -= fracOneLow;
            }

            // Loop as long as the outpos trails behind, and as long as there is
            // still space in the output buffer.
            while (opos < fracOneLow && obuf < oend)
            {
                // interpolate
                int16_t out0, out1;
                out0 = static_cast<int16_t>(ilast0 + (((icur0 - ilast0) * opos + fracHalfLow) >> fracBitsLow));
                out1 = (stereo ?
                    static_cast<int16_t>(ilast1 + (((icur1 - ilast1) * opos + fracHalfLow) >> fracBitsLow)) :
                    out0);

                // output left channel
                utils::clampAdd(obuf[reverseStereo], (out0 * static_cast<int>(vol_l)) / scummvm::Mixer::MaxVolume::MIXER);

                // output right channel
                utils::clampAdd(obuf[reverseStereo ^ 1], (out1 * static_cast<int>(vol_r)) / scummvm::Mixer::MaxVolume::MIXER);

                obuf += 2;

                // Increment output position
                opos += opos_inc;
            }
        }

        return (obuf - ostart) / 2;
    }
}
