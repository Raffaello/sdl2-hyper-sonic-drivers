#pragma once

#include <audio/scummvm/Mixer.hpp>
#include <audio/Sound.hpp>
#include <memory>
#include <cstdint>

namespace drivers
{
    class PCMDriver final
    {
    public:
        PCMDriver(std::shared_ptr<audio::scummvm::Mixer> mixer);
        
        bool isPlaying(const std::shared_ptr<audio::Sound> sound) const noexcept;
        void play(const std::shared_ptr<audio::Sound> sound, const uint8_t volume = audio::scummvm::Mixer::MaxVolume::CHANNEL);
    private:
        std::shared_ptr<audio::scummvm::Mixer> _mixer;
    };
}
