#pragma once

#include <cstdint>
#include <HyperSonicDrivers/audio/scummvm/RateConverter.hpp>
#include <HyperSonicDrivers/audio/scummvm/AudioStream.hpp>

#include <SDL2/SDL_log.h>

namespace HyperSonicDrivers::audio::converters
{
    /**
     * Simple audio rate converter for the case that the inrate equals the outrate.
     */
    template<bool stereo, bool reverseStereo>
    class CopyRateConverter : public scummvm::RateConverter {
        int16_t* _buffer;
        uint32_t _bufferSize;
    public:
        CopyRateConverter() : _buffer(0), _bufferSize(0) {}
        ~CopyRateConverter() {
            free(_buffer);
        }

        virtual int flow(scummvm::AudioStream& input, int16_t* obuf, uint32_t osamp, uint16_t vol_l, uint16_t vol_r) {
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
                scummvm::clampedAdd(obuf[reverseStereo], (out0 * (int)vol_l) / scummvm::Mixer::MaxVolume::MIXER);

                // output right channel
                scummvm::clampedAdd(obuf[reverseStereo ^ 1], (out1 * (int)vol_r) / scummvm::Mixer::MaxVolume::MIXER);

                obuf += 2;
            }
            return (obuf - ostart) / 2;
        }

        virtual int drain(int16_t* obuf, uint32_t osamp, uint16_t vol) {
            return scummvm::ST_SUCCESS;
        }
    };
}
