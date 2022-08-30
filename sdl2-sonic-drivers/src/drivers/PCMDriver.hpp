#pragma once

#include <audio/scummvm/Mixer.hpp>
#include <audio/streams/SoundStream.hpp>
#include <audio/Sound.hpp>
#include <memory>
#include <cstdint>
#include <vector>

namespace drivers
{
    /**
     * @brief Managing PCM sound and their playback
     * @details It is a sort of bridge between Mixer and Sound objects
     *          so just simplify playing digital sounds.
     */
    class PCMDriver final
    {
    public:
        // TODO: review max_channels default value
        // BODY: can use max_channel = 0 to be dynamic instead of fixed size
        // BODY: can be -1 (check if < 0) to use mixer max channels
        PCMDriver(const std::shared_ptr<audio::scummvm::Mixer>& mixer, const int max_channels = 0xFFFF);
        ~PCMDriver();

        bool isPlaying() const noexcept;
        bool isPlaying(const std::shared_ptr<audio::Sound>& sound) const noexcept;
        void play(const std::shared_ptr<audio::Sound>& sound, const uint8_t volume = audio::scummvm::Mixer::MaxVolume::CHANNEL, const int8_t balance = 0);
    private:
        std::shared_ptr<audio::scummvm::Mixer> _mixer;
        std::vector<std::shared_ptr<audio::streams::SoundStream>> _soundStreams;
        int _max_streams;

        inline bool isSoundHandleActive(const int index) const noexcept;
    };
}
