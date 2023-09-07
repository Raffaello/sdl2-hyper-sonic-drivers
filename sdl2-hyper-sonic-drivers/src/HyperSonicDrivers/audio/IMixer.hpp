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
            const int8_t pan,
            const bool reverseStereo
        ) = 0;

        virtual void suspend() noexcept = 0;
        virtual void resume() noexcept = 0;

        virtual void reset() noexcept = 0;
        virtual void reset(const uint8_t id) noexcept = 0;

        virtual void pause() noexcept = 0;
        virtual void pause(const uint8_t id) noexcept = 0;

        virtual void unpause() noexcept = 0;
        virtual void unpause(const uint8_t id) noexcept = 0;

        virtual bool isChannelActive(const uint8_t id) const noexcept = 0;
        virtual bool isPaused(const uint8_t id) const noexcept = 0;

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

        // TODO: these 3 methods are useless if those 3 vars are consts...
        inline uint32_t getOutputRate() const noexcept { return m_sampleRate; };
        inline uint16_t getBufferSize() const noexcept { return m_samples; };
        inline uint8_t getBitsDepth() const noexcept { return m_bitsDepth; };

        const uint8_t max_channels;
    protected:
        std::array<mixer::channelGroupSettings_t, mixer::eChannelGroup_size> m_group_settings;
        bool m_ready = false;
        const uint32_t m_sampleRate;
        const uint16_t m_samples;
        const uint8_t m_bitsDepth = 16; // forced to be 16-bits for now
    };

    template<class T, typename... Args>
    std::shared_ptr<IMixer> make_mixer(Args... args)
    {
        return std::make_shared<T>(args...);
    }
}
