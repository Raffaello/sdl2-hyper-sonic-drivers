#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <memory>
#include <HyperSonicDrivers/audio/mixer/ChannelGroup.hpp>
#include <HyperSonicDrivers/audio/IAudioStream.hpp>

namespace HyperSonicDrivers::audio
{
    class IMixer
    {
    public:
        IMixer(IMixer&) = delete;
        IMixer& operator=(IMixer&) = delete;

        IMixer(const uint8_t max_channels);
        virtual ~IMixer() = default;

        virtual bool init() = 0;

        inline bool isReady() const noexcept { return m_ready; };
 
        virtual void play(
            const mixer::eChannelGroup group,
            const std::shared_ptr<IAudioStream>& stream,
            const uint8_t vol,
            const int8_t pan,
            const bool reverseStereo
        ) = 0;
        
        virtual void stop() noexcept = 0;
        virtual void stop(const uint8_t id) noexcept = 0;
        //virtual void stop(Handle )
        
        virtual void pause() noexcept = 0;
        virtual void pause(const uint8_t id) noexcept = 0;
        //virtual void pause(const SoundHandle) = 0;

        virtual void unpause() noexcept = 0;
        virtual void unpause(const uint8_t id) noexcept = 0;

        virtual bool isChannelActive(const uint8_t id) const noexcept = 0;
        //virtual void isChannelActive(Handle) = 0;
        
        //virtual void getChannelId(handle)

        virtual bool isChannelGroupMuted(const mixer::eChannelGroup group) const noexcept = 0;
        virtual void muteChannelGroup(const mixer::eChannelGroup group) noexcept = 0;
        virtual void unmuteChannelGroup(const mixer::eChannelGroup group) noexcept = 0;

        virtual uint8_t getChannelVolume(const uint8_t id) const noexcept = 0;
        virtual void setChannelVolume(const uint8_t id, const uint8_t volume) noexcept = 0;
        virtual uint8_t getChannelPan(const uint8_t id) const noexcept = 0;
        virtual void setChannelPan(const uint8_t id, const int8_t pan) noexcept = 0;

        virtual void setChannelVolumePan(const uint8_t id, const uint8_t volume, const int8_t pan) noexcept = 0;

        uint8_t getChannelGroupVolume(const mixer::eChannelGroup group) const noexcept;
        void setChannelGroupVolume(const mixer::eChannelGroup group, const uint8_t volume) noexcept;

        inline uint32_t getOutputRate() const noexcept { return m_sampleRate; };
        inline uint8_t getBitsDepth() const noexcept { return m_bitsDepth; };

        const uint8_t max_channels;
    protected:
        std::array<mixer::channelGroupSettings_t, mixer::eChannelGroup_size> m_group_settings;
        //std::vector<mixer::Channel> m_channels;
        bool m_ready = false;
        uint32_t m_sampleRate;
        uint8_t m_bitsDepth;
    };
}
