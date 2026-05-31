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
template <bool stereo, bool reverseStereo>
class SimpleRateConverter : public IRateConverter
{
private:
    std::array<int16_t, intermediateBufferSize> inBuf = {};
    const int16_t*                              inPtr = nullptr;
    size_t                                      inLen = 0;

    /** position of how far output is ahead of input */
    /** Holds what would have been out_pos-ipos */
    long m_out_pos = 0;

    /** fractional position increment in the output stream */
    long m_out_pos_inc = 0;

public:
    SimpleRateConverter(uint32_t in_rate, uint32_t outrate);
    size_t flow(IAudioStream& input, int16_t* obuf, uint32_t osamp, const uint16_t vol_l, const uint16_t vol_r) override;
};

/*
 * Prepare processing.
 */
template <bool stereo, bool reverseStereo>
SimpleRateConverter<stereo, reverseStereo>::SimpleRateConverter(uint32_t in_rate, uint32_t outrate)
{
    if ((in_rate % outrate) != 0)
    {
        utils::logE("Input rate must be a multiple of output rate to use rate effect");
    }

    if (in_rate >= 65536 || outrate >= 65536)
    {
        utils::logE("rate effect can only handle rates < 65536");
    }

    m_out_pos = 1;

    /* increment */
    m_out_pos_inc = in_rate / outrate;

    inLen = 0;
}

/*
 * Processed signed long samples from ibuf to obuf.
 * Return number of sample pairs processed.
 */
template <bool stereo, bool reverseStereo>
size_t SimpleRateConverter<stereo, reverseStereo>::flow(IAudioStream& input, int16_t* obuf, uint32_t osamp, const uint16_t vol_l, const uint16_t vol_r)
{
    const int16_t* out_start = obuf;
    const int16_t* oend      = obuf + osamp * 2;

    while (obuf < oend)
    {
        // read enough input samples so that out_pos >= 0
        do
        {
            // Check if we have to refill the buffer
            if (inLen == 0)
            {
                inPtr = inBuf.data();
                inLen = input.readBuffer(inBuf.data(), inBuf.size());
                if (inLen <= 0)
                    return (obuf - out_start) / 2;
            }
            inLen -= (stereo ? 2 : 1);
            m_out_pos--;
            if (m_out_pos >= 0)
            {
                inPtr += (stereo ? 2 : 1);
            }
        }
        while (m_out_pos >= 0);

        int16_t out0 = *inPtr++;
        int16_t out1 = (stereo ? *inPtr++ : out0);

        // Increment output position
        m_out_pos += m_out_pos_inc;

        // output left channel
        output_channel(obuf[reverseStereo ? 1 : 0], out0, vol_l);
        // output right channel
        output_channel(obuf[reverseStereo ? 0 : 1], out1, vol_r);
        obuf += 2;
    }

    return (obuf - out_start) / 2;
}
}    // namespace HyperSonicDrivers::audio::converters
