#pragma once

#include <cstdint>
#include <memory>
#include <cassert>
#include <HyperSonicDrivers/audio/converters/IRateConverter.hpp>
#include <HyperSonicDrivers/audio/scummvm/AudioStream.hpp>
#include <HyperSonicDrivers/utils/algorithms.hpp>
#include <HyperSonicDrivers/audio/scummvm/Mixer.hpp>

#include <SDL2/SDL_log.h>

namespace HyperSonicDrivers::audio::converters
{
    /**
     * Simple audio rate converter for the case that the inrate equals the outrate.
     */
    template<bool stereo, bool reverseStereo>
    class CopyRateConverter : public IRateConverter
    {
    private:
        std::unique_ptr<int16_t[]> _buffer = nullptr;
        uint32_t _bufferSize = 0;

    public:
        CopyRateConverter() = default;
        virtual ~CopyRateConverter() = default;

        int flow(scummvm::AudioStream& input, int16_t* obuf, uint32_t osamp, const uint16_t vol_l, const uint16_t vol_r) override
        {
            assert(input.isStereo() == stereo);

            int16_t* ptr;
            uint32_t len;

            int16_t* ostart = obuf;

            if (stereo)
                osamp *= 2;

            // Reallocate temp buffer, if necessary
            if (osamp > _bufferSize)
            {
                _buffer = std::make_unique<int16_t[]>(osamp);
                _bufferSize = osamp;
            }

            if (_buffer == nullptr)
            {
                SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "[CopyRateConverter::flow] Cannot allocate memory for temp buffer");
                return 0;
            }

            // Read up to 'osamp' samples into our temporary buffer
            len = input.readBuffer(_buffer.get(), osamp);

            // Mix the data into the output buffer
            ptr = _buffer.get();

            for (; len > 0; len -= (stereo ? 2 : 1))
            {
                int16_t out0, out1;
                out0 = *ptr++;
                out1 = (stereo ? *ptr++ : out0);

                // output left channel
                utils::clampAdd(obuf[reverseStereo], (out0 * static_cast<int>(vol_l)) / scummvm::Mixer::MaxVolume::MIXER);
                // output right channel
                utils::clampAdd(obuf[reverseStereo ^ 1], (out1 * static_cast<int>(vol_r)) / scummvm::Mixer::MaxVolume::MIXER);

                obuf += 2;
            }

            return (obuf - ostart) / 2;
        }

        int drain(int16_t* obuf, uint32_t osamp, const uint16_t vol) override
        {
            return 0;
        }
    };
}
