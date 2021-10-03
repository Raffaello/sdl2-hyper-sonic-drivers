#pragma once

#include <audio/scummvm/Mixer.hpp>
#include <audio/streams/SoundStream.hpp>
#include <audio/Sound.hpp>
#include <memory>
#include <cstdint>
#include <vector>

namespace drivers
{
    class PCMDriver final
    {
    public:
        PCMDriver(std::shared_ptr<audio::scummvm::Mixer> mixer, const int max_channels = 0xFFFF);
        ~PCMDriver();

        bool isPlaying() const noexcept;
        bool isPlaying(const std::shared_ptr<audio::Sound> sound) const noexcept;
        void play(const std::shared_ptr<audio::Sound> sound, const uint8_t volume = audio::scummvm::Mixer::MaxVolume::CHANNEL, const int8_t balance = 0);
    private:
        std::shared_ptr<audio::scummvm::Mixer> _mixer;
        std::vector<std::shared_ptr<audio::streams::SoundStream>> _soundStreams;
        int _max_streams;

        inline bool isSoundHandleActive(const int index) const noexcept;
    };
}
