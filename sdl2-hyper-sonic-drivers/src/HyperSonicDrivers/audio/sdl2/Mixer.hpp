#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <mutex>
#include <HyperSonicDrivers/audio/IMixer.hpp>
#include <HyperSonicDrivers/audio/mixer/Channel.hpp>

namespace HyperSonicDrivers::audio::sdl2
{
    class Mixer : public IMixer
    {
    public:
        Mixer(const uint8_t max_channels);

        bool init() override;

        void play(
            const mixer::eChannelGroup group,
            const std::shared_ptr<IAudioStream>& stream,
            const uint8_t vol,
            const int8_t pan,
            const bool reverseStereo
        ) override;

        void stop() noexcept override;
        void stop(const uint8_t id) noexcept override;

        void pause() noexcept override;
        void pause(const uint8_t id) noexcept override;

        void unpause() noexcept override;
        void unpause(const uint8_t id) noexcept override;

        bool isChannelActive(const uint8_t id) const noexcept override;

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

        mutable std::mutex m_mutex;
        std::vector<std::unique_ptr<mixer::Channel>> m_channels;
        
    };
}
