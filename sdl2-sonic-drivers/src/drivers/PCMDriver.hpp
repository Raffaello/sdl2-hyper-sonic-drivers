#pragma once

#include <audio/scummvm/Mixer.hpp>
#include <audio/scummvm/SoundHandle.hpp>
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
        
        bool isPlaying(const std::shared_ptr<audio::Sound> sound) const noexcept;
        void play(const std::shared_ptr<audio::Sound> sound, const uint8_t volume = audio::scummvm::Mixer::MaxVolume::CHANNEL);
    private:
        std::shared_ptr<audio::scummvm::Mixer> _mixer;
        typedef struct channel_t
        {
            audio::scummvm::SoundHandle h;
            std::shared_ptr<audio::Sound> s;
            int buf_pos = 0;
        } channel_t;

        std::vector<channel_t> _handles;
        int _max_handles;
        int _cur_handles;

    };
}
