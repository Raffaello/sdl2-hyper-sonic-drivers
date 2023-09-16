#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <HyperSonicDrivers/audio/IMixer.hpp>
#include <HyperSonicDrivers/audio/IAudioStream.hpp>
#include <HyperSonicDrivers/audio/scummvm/Timestamp.hpp>

namespace HyperSonicDrivers::audio::stubs
{
    class StubMixer : public IMixer
    {
    public:
        int rate = 44100;

        StubMixer() : IMixer(32, 44100, 1024/*, const uint8_t bitsDepth*/) {};

        bool init() override { return true; };

        std::optional<uint8_t> play(
            const mixer::eChannelGroup group,
            const std::shared_ptr<IAudioStream>& stream,
            const uint8_t vol,
            const int8_t pan
        ) override {
            return std::make_optional(0);
        };

        void suspend() noexcept override {};
        void resume() noexcept override {};

        void reset() noexcept override {};
        void reset(const uint8_t id) noexcept override {};

        void pause() noexcept override {};
        void pause(const uint8_t id) noexcept override {};
        
        void unpause() noexcept override {};
        void unpause(const uint8_t id) noexcept override {};

        bool isChannelActive(const uint8_t id) const noexcept override { return true; };
        bool isPaused(const uint8_t id) const noexcept override { return false; }
        bool isChannelGroupMuted(const mixer::eChannelGroup group) const noexcept override { return false; };
        void muteChannelGroup(const mixer::eChannelGroup group) noexcept override {};
        void unmuteChannelGroup(const mixer::eChannelGroup group) noexcept override {};

        uint8_t getChannelVolume(const uint8_t id) const noexcept override { return audio::mixer::Channel_max_volume; };
        void setChannelVolume(const uint8_t id, const uint8_t volume) noexcept override {};
        uint8_t getChannelPan(const uint8_t id) const noexcept override { return 0; };
        void setChannelPan(const uint8_t id, const int8_t pan) noexcept override {};

        void setChannelVolumePan(const uint8_t id, const uint8_t volume, const int8_t pan) noexcept override {};

        void setMasterVolume(const uint8_t master_volume) noexcept override {};
    };
}
