#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <memory>
#include <optional>
#include <HyperSonicDrivers/audio/mixer/ChannelGroup.hpp>
#include <HyperSonicDrivers/audio/IAudioStream.hpp>

namespace HyperSonicDrivers::audio
{
    /**
    * 16 bit signed, stereo
    **/
    class IMixer
    {
    public:
        const uint8_t max_channels;
        const uint32_t freq;
        const uint16_t buffer_size;
        const uint8_t bitsDepth = 16; // forced to be 16-bits for now

        IMixer(IMixer&) = delete;
        IMixer& operator=(IMixer&) = delete;

        IMixer(const uint8_t max_channels, const uint32_t freq, const uint16_t buffer_size/*, const uint8_t bitsDepth*/);
        virtual ~IMixer() = default;

        virtual bool init() = 0;
        inline bool isReady() const noexcept { return m_ready; };

        /**
        * returns channel id used to play the stream
        **/
        virtual std::optional<uint8_t> play(
            const mixer::eChannelGroup group,
            const std::shared_ptr<IAudioStream>& stream,
            const uint8_t vol,
            const int8_t pan
        ) = 0;

        virtual void suspend() noexcept = 0;
        virtual void resume() noexcept = 0;

        virtual void reset() noexcept = 0;
        virtual void reset(const uint8_t id) noexcept = 0;
        virtual void reset(const mixer::eChannelGroup group) noexcept = 0;

        virtual void pause() noexcept = 0;
        virtual void pause(const uint8_t id) noexcept = 0;

        virtual void unpause() noexcept = 0;
        virtual void unpause(const uint8_t id) noexcept = 0;

        virtual bool isActive(const uint8_t id) const noexcept = 0;
        virtual bool isPaused(const uint8_t id) const noexcept = 0;

        virtual bool isActive() const noexcept = 0;
        virtual bool isActive(const mixer::eChannelGroup group) = 0;

        virtual bool isChannelGroupMuted(const mixer::eChannelGroup group) const noexcept = 0;
        virtual void muteChannelGroup(const mixer::eChannelGroup group) noexcept = 0;
        virtual void unmuteChannelGroup(const mixer::eChannelGroup group) noexcept = 0;

        virtual uint8_t getChannelVolume(const uint8_t id) const noexcept = 0;
        virtual void setChannelVolume(const uint8_t id, const uint8_t volume) noexcept = 0;
        virtual uint8_t getChannelPan(const uint8_t id) const noexcept = 0;
        virtual void setChannelPan(const uint8_t id, const int8_t pan) noexcept = 0;

        virtual void setChannelVolumePan(const uint8_t id, const uint8_t volume, const int8_t pan) noexcept = 0;

        virtual mixer::eChannelGroup getChannelGroup(const uint8_t id) const noexcept = 0;

        uint8_t getChannelGroupVolume(const mixer::eChannelGroup group) const noexcept;
        void setChannelGroupVolume(const mixer::eChannelGroup group, const uint8_t volume) noexcept;

        int8_t getChannelGroupPan(const mixer::eChannelGroup group) const noexcept;
        void setChannelGroupPan(const mixer::eChannelGroup group, const int8_t pan) noexcept;

        inline uint8_t getMasterVolume() const noexcept { return m_master_volume; };
        virtual void setMasterVolume(const uint8_t master_volume) noexcept = 0;

        inline void toggleReverseStereo() noexcept { m_reverseStereo = !m_reverseStereo; };

    protected:
        virtual void updateChannelsVolumePan_() noexcept = 0;

        std::array<mixer::channelGroupSettings_t, mixer::eChannelGroup_size> m_group_settings;
        bool m_ready = false; // TODO: not really useful if not used anywhere else except init.
                              //       unless remove init method and do it in the constructor
                              //       and then check if it is ready before use the mixer
                              //       otherwise can just be removed.
        bool m_reverseStereo = false;
        
        uint8_t m_master_volume = mixer::Mixer_max_volume;
    };

    template<class T, typename... Args>
    std::shared_ptr<IMixer> make_mixer(Args... args)
    {
        return std::make_shared<T>(args...);
    }
}
