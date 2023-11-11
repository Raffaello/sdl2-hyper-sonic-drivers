#pragma once

#include <cstdint>
#include <array>
#include <HyperSonicDrivers/audio/converters/IRateConverter.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>

namespace HyperSonicDrivers::audio::converters
{
    constexpr int16_t interpolate(const int16_t a, const int16_t b, const int32_t t)
    {
        return static_cast<int16_t>(a + (((b - a) * t + fracHalfLow) >> fracBitsLow));
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
    class LinearRateConverter : public IRateConverter
    {
    private:
        std::array<int16_t, intermediateBufferSize> inBuf = {};
        const int16_t* inPtr = nullptr;
        size_t inLen = 0;

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
        size_t flow(IAudioStream& input, int16_t* obuf, uint32_t osamp, const uint16_t vol_l, const uint16_t vol_r) override;
    };


    /*
     * Prepare processing.
     */
    template<bool stereo, bool reverseStereo>
    LinearRateConverter<stereo, reverseStereo>::LinearRateConverter(uint32_t inrate, uint32_t outrate)
    {
        if (inrate >= 131072 || outrate >= 131072)
        {
            utils::logE("rate effect can only handle rates < 131072");
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
    size_t LinearRateConverter<stereo, reverseStereo>::flow(IAudioStream& input, int16_t* obuf, uint32_t osamp, const uint16_t vol_l, const uint16_t vol_r)
    {
        const int16_t* ostart = obuf;
        const int16_t* oend = obuf + osamp * 2;

        while (obuf < oend)
        {
            // read enough input samples so that opos < 0
            while (fracOneLow <= opos)
            {
                // Check if we have to refill the buffer
                if (inLen == 0)
                {
                    inPtr = inBuf.data();
                    inLen = input.readBuffer(inBuf.data(), static_cast<int>(inBuf.size()));
                    if (inLen <= 0)
                        return (obuf - ostart) / 2;
                }
                inLen -= (stereo ? 2 : 1);
                ilast0 = icur0;
                icur0 = *inPtr++;
                if constexpr (stereo)
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
                const int16_t out0 = interpolate(ilast0, icur0, opos);
                const int16_t out1 = stereo ? interpolate(ilast1, icur1, opos) : out0;
                // output left channel
                output_channel(obuf[reverseStereo ? 0 : 1], out0, vol_l);
                // output right channel
                output_channel(obuf[reverseStereo ? 1 : 0], out1, vol_r);
                obuf += 2;

                // Increment output position
                opos += opos_inc;
            }
        }

        return (obuf - ostart) / 2;
    }
}
