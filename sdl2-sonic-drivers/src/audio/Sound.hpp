#pragma once

#include <audio/scummvm/Mixer.hpp>
#include <cstdint>
#include <memory>

namespace audio
{
    class Sound final
    {
    public:
        Sound(const Sound&) = delete;
        Sound(const scummvm::Mixer::SoundType soundType, const bool isStereo, const int rate, const uint8_t bitsDepth, const uint32_t dataSize, const std::shared_ptr<uint8_t[]> data);

        const scummvm::Mixer::SoundType soundType;
        const int rate;
        const bool stereo;
        const uint8_t bitsDepth;
        const uint32_t dataSize;
        const std::shared_ptr<uint8_t[]> data;

    private:
        
    };
}
