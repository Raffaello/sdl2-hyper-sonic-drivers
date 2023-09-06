#pragma once

#include <cstdint>
#include <cstddef>
#include <memory>
#include <HyperSonicDrivers/audio/IMixer.hpp>
#include <HyperSonicDrivers/audio/converters/IRateConverter.hpp>
#include <HyperSonicDrivers/audio/IAudioStream.hpp>
#include <HyperSonicDrivers/audio/mixer/ChannelGroup.hpp>


namespace HyperSonicDrivers::audio::mixer
{
    class Channel
    {
    public:
        Channel(Channel&) = delete;
        Channel(Channel&&) = delete;
        Channel& operator=(Channel&) = delete;

        Channel(IMixer& mixer, const uint8_t id);
        ~Channel() = default;

        void setAudioStream(const mixer::eChannelGroup group, const std::shared_ptr<IAudioStream>& stream,
            const bool reverseStereo);

        void setAudioStream(const mixer::eChannelGroup group, const std::shared_ptr<IAudioStream>& stream,
            const uint8_t volume,
            const int8_t pan,
            const bool reverseStereo);

        /**
         * Mixes the channel's samples into the given buffer.
         *
         * @param data buffer where to mix the data
         * @param len  number of sample *pairs*. So a value of
         *             10 means that the buffer contains twice 10 sample, each
         *             16 bits, for a total of 40 bytes.
         * @return number of sample pairs processed (which can still be silence!)
         */
        size_t mix(int16_t* data, size_t len);

        inline bool isEnded() const noexcept { return m_stream == nullptr; };
        inline int getId() const noexcept { return m_id; };

        void pause() noexcept;
        void unpause() noexcept;

        void stop() noexcept;

        inline bool isPaused() const noexcept { return m_pause; };

        inline uint8_t getVolume() const noexcept { return m_volume; };
        inline int8_t getPan() const noexcept { return m_pan; };

        void setVolume(const uint8_t volume);
        void setPan(const int8_t pan);
        void setVolumePan(const uint8_t volume, const uint8_t pan);

        inline mixer::eChannelGroup getChannelGroup() const noexcept { return m_group; };

        void updateVolumePan();

        /**
         * Queries how long the channel has been playing.
         */
        //Timestamp getElapsedTime();

        /**
         * Sets the channel's sound handle.
         *
         * @param handle new handle
         */
        //void setHandle(const SoundHandle handle) { _handle = handle; }

        /**
         * Queries the channel's sound handle.
         */
        //SoundHandle getHandle() const { return _handle; }

    private:
        IMixer& m_mixer;
        const uint8_t m_id;
        mixer::eChannelGroup m_group = mixer::eChannelGroup::Unknown;
        //SoundHandle _handle;
        //bool _permanent;

        uint8_t m_volume = 0;
        int8_t m_pan = 0;
        bool m_pause = false;

        uint16_t m_volL = 0;
        uint16_t m_volR = 0;

        //uint32_t m_samplesConsumed;
        //uint32_t m_samplesDecoded;
        //uint32_t m_mixerTimeStamp;
        uint32_t m_pauseStartTime = 0;
        uint32_t m_pauseTime = 0;

        std::unique_ptr<converters::IRateConverter> m_converter;
        std::shared_ptr<IAudioStream> m_stream;
    };
}
