#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <mutex>
#include <optional>
#include <HyperSonicDrivers/audio/IMixer.hpp>
#include <HyperSonicDrivers/audio/mixer/Channel.hpp>
#include <SDL2/SDL_audio.h>

namespace HyperSonicDrivers::audio::sdl2
{
    class Mixer : public IMixer
    {
    public:
        Mixer(const uint8_t max_channels,
            const uint32_t freq, const uint16_t buffer_size);
        ~Mixer() override;

        bool init() override;

        std::optional<uint8_t> play(
            const mixer::eChannelGroup group,
            const std::shared_ptr<IAudioStream>& stream,
            const uint8_t vol,
            const int8_t pan,
            const bool reverseStereo
        ) override;

        void suspend() noexcept override;
        void resume() noexcept override;

        void reset() noexcept override;
        void reset(const uint8_t id) noexcept override;

        void pause() noexcept override;
        void pause(const uint8_t id) noexcept override;

        void unpause() noexcept override;
        void unpause(const uint8_t id) noexcept override;

        bool isChannelActive(const uint8_t id) const noexcept override;
        bool isPaused(const uint8_t id) const noexcept override;


        bool isChannelGroupMuted(const mixer::eChannelGroup group) const noexcept override;;
        void muteChannelGroup(const mixer::eChannelGroup group) noexcept override;
        void unmuteChannelGroup(const mixer::eChannelGroup group) noexcept override;

        uint8_t getChannelVolume(const uint8_t id) const noexcept override;
        void setChannelVolume(const uint8_t id, const uint8_t volume) noexcept override;
        uint8_t getChannelPan(const uint8_t id) const noexcept override;
        void setChannelPan(const uint8_t id, const int8_t pan) noexcept override;

        void setChannelVolumePan(const uint8_t id, const uint8_t volume, const int8_t pan) noexcept override;

    protected:
        void updateChannelsVolumePan_() noexcept;

        size_t callback(uint8_t* samples, unsigned int len);
        static void sdlCallback(void* this_, uint8_t* samples, int len);

        SDL_AudioDeviceID m_device_id = 0;
        mutable std::mutex m_mutex;
        std::vector<std::unique_ptr<mixer::Channel>> m_channels;
        
    };
}
