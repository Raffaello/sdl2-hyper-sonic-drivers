#pragma once

#include <memory>
#include <cstdint>
#include <vector>
#include <HyperSonicDrivers/audio/IMixer.hpp>
#include <HyperSonicDrivers/audio/streams/SoundStream.hpp>
#include <HyperSonicDrivers/audio/Sound.hpp>
#include <HyperSonicDrivers/audio/mixer/config.hpp>

namespace HyperSonicDrivers::drivers
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
        explicit PCMDriver(const std::shared_ptr<audio::IMixer>& mixer, const uint8_t max_channels = 0xFFFF);
        ~PCMDriver() = default;

        bool isPlaying() const noexcept;
        bool isPlaying(const std::shared_ptr<audio::Sound>& sound) const noexcept;
        void play(const std::shared_ptr<audio::Sound>& sound, const uint8_t volume = audio::mixer::Channel_max_volume, const int8_t pan = 0);
    private:
        std::shared_ptr<audio::IMixer> m_mixer;
        std::vector<std::shared_ptr<audio::streams::SoundStream>> m_soundStreams;
        uint8_t m_max_streams;

        //inline bool isSoundHandleActive(const int index) const noexcept;
    };
}
