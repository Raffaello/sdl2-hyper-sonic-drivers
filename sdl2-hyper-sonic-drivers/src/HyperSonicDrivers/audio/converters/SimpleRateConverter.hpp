#pragma once

#include <cstdint>
#include <array>
#include <HyperSonicDrivers/audio/converters/IRateConverter.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>

namespace HyperSonicDrivers::audio::converters
{
    /**
     * Audio rate converter based on simple resampling. Used when no
     * interpolation is required.
     *
     * Limited to sampling frequency <= 65535 Hz.
    **/
    template<bool stereo, bool reverseStereo>
    class SimpleRateConverter : public IRateConverter
    {
    private:
        std::array<int16_t, intermediateBufferSize> inBuf = {};
        const int16_t* inPtr = nullptr;
        size_t inLen = 0;

        /** position of how far output is ahead of input */
        /** Holds what would have been opos-ipos */
        long opos = 0;

        /** fractional position increment in the output stream */
        long opos_inc = 0;

    public:
        SimpleRateConverter(uint32_t inrate, uint32_t outrate);
        size_t flow(IAudioStream& input, int16_t* obuf, uint32_t osamp, const uint16_t vol_l, const uint16_t vol_r) override;
    };

    /*
     * Prepare processing.
     */
    template<bool stereo, bool reverseStereo>
    SimpleRateConverter<stereo, reverseStereo>::SimpleRateConverter(uint32_t inrate, uint32_t outrate)
    {
        if ((inrate % outrate) != 0)
        {
            utils::logE("Input rate must be a multiple of output rate to use rate effect");
        }

        if (inrate >= 65536 || outrate >= 65536)
        {
            utils::logE("rate effect can only handle rates < 65536");
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
    size_t SimpleRateConverter<stereo, reverseStereo>::flow(IAudioStream& input, int16_t* obuf, uint32_t osamp, const uint16_t vol_l, const uint16_t vol_r)
    {
        const int16_t* ostart = obuf;
        const int16_t* oend = obuf + osamp * 2;

        while (obuf < oend)
        {
            // read enough input samples so that opos >= 0
            do
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
                opos--;
                if (opos >= 0)
                {
                    inPtr += (stereo ? 2 : 1);
                }
            } while (opos >= 0);

            int16_t out0 = *inPtr++;
            int16_t out1 = (stereo ? *inPtr++ : out0);

            // Increment output position
            opos += opos_inc;

            // output left channel
            output_channel(obuf[reverseStereo ? 1 : 0], out0, vol_l);
            // output right channel
            output_channel(obuf[reverseStereo ? 0 : 1], out1, vol_r);
            obuf += 2;
        }

        return (obuf - ostart) / 2;
    }
}
