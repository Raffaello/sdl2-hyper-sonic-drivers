#pragma once

#include <HyperSonicDrivers/audio/IMixer.hpp>
#include <HyperSonicDrivers/audio/mixer/Channel.hpp>
#include <HyperSonicDrivers/audio/mixer/ChannelGroup.hpp>

#include <mutex>
#include <vector>
#include <memory>
#include <cstdint>
#include <optional>

namespace HyperSonicDrivers::audio::mixer
{

class MixerBase : public IMixer
{
public:
    MixerBase(const uint8_t  max_channels,
              const uint32_t freq,
              const uint16_t buffer_size);
    ~MixerBase() override;

    bool init() override final;
    void shutdown() override final;

    std::optional<uint8_t> play(
        const mixer::eChannelGroup           group,
        const std::shared_ptr<IAudioStream>& stream,
        const uint8_t                        vol,
        const int8_t                         pan) override;

    // void suspend() noexcept override;
    // void resume() noexcept override;

    void reset() noexcept override;
    void reset(const uint8_t id) noexcept override;
    void reset(const mixer::eChannelGroup group) noexcept override;

    void pause() noexcept override;
    void pause(const uint8_t id) noexcept override;

    void unpause() noexcept override;
    void unpause(const uint8_t id) noexcept override;

    bool isActive(const uint8_t id) const noexcept override;
    bool isPaused(const uint8_t id) const noexcept override;

    bool isActive() const noexcept override;
    bool isActive(const mixer::eChannelGroup group) const noexcept override;

    bool isChannelGroupMuted(const mixer::eChannelGroup group) const noexcept override;
    void muteChannelGroup(const mixer::eChannelGroup group) noexcept override;
    void unmuteChannelGroup(const mixer::eChannelGroup group) noexcept override;

    uint8_t getChannelVolume(const uint8_t id) const noexcept override;
    void    setChannelVolume(const uint8_t id, const uint8_t volume) noexcept override;
    uint8_t getChannelPan(const uint8_t id) const noexcept override;
    void    setChannelPan(const uint8_t id, const int8_t pan) noexcept override;

    void setChannelVolumePan(const uint8_t id, const uint8_t volume, const int8_t pan) noexcept override;

    mixer::eChannelGroup getChannelGroup(const uint8_t id) const noexcept override;

    void setMasterVolume(const uint8_t master_volume) noexcept override;

protected:
    mutable std::mutex                           m_mutex;
    std::vector<std::unique_ptr<mixer::Channel>> m_channels;

    void         updateChannelsVolumePan_() noexcept override;
    virtual bool onInit_()     = 0;
    virtual void onShutdown_() = 0;
};

}    // namespace HyperSonicDrivers::audio::mixer
