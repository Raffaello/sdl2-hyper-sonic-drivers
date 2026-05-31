#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <cassert>
#include <HyperSonicDrivers/audio/converters/IRateConverter.hpp>

namespace HyperSonicDrivers::audio::converters
{
/**
 * Simple audio rate converter for the case that the in_rate equals the outrate.
 */
template <bool stereo, bool reverseStereo>
class CopyRateConverter : public IRateConverter
{
private:
    std::vector<int16_t> _buffer;

public:
    CopyRateConverter()           = default;
    ~CopyRateConverter() override = default;

    size_t flow(IAudioStream& input, int16_t* obuf, uint32_t osamp, const uint16_t vol_l, const uint16_t vol_r) override
    {
        assert(input.isStereo() == stereo);

        if (stereo)
            osamp *= 2;

        // Reallocate temp buffer, if necessary
        if (osamp > _buffer.size())
            _buffer.resize(osamp);

        const int16_t* out_start = obuf;
        // Read up to 'osamp' samples into our temporary buffer
        size_t len = input.readBuffer(_buffer.data(), osamp);

        // Mix the data into the output buffer
        auto it = _buffer.begin();
        for (; len > 0; len -= (stereo ? 2 : 1))
        {
            int16_t out0 = *it++;
            int16_t out1 = (stereo ? *it++ : out0);

            // output left channel
            output_channel(obuf[reverseStereo ? 1 : 0], out0, vol_l);
            // output right channel
            output_channel(obuf[reverseStereo ? 0 : 1], out1, vol_r);
            obuf += 2;
        }

        return (obuf - out_start) / 2;
    }
};
}    // namespace HyperSonicDrivers::audio::converters
