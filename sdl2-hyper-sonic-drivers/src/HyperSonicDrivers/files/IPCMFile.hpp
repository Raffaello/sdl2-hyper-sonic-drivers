#pragma once

#include <cstdint>
#include <memory>
#include <HyperSonicDrivers/audio/Sound.hpp>
#include <HyperSonicDrivers/audio/mixer/ChannelGroup.hpp>

namespace HyperSonicDrivers::files
{
    class IPCMFile
    {
    public:
        IPCMFile(IPCMFile&) = delete;
        IPCMFile(IPCMFile&&) = delete;
        IPCMFile& operator=(IPCMFile&) = delete;

        IPCMFile() = default;
        virtual ~IPCMFile() = default;

        inline int getChannels() const noexcept { return m_channels; };
        inline uint32_t getSampleRate() const noexcept { return m_sampleRate; };
        inline uint8_t getBitsDepth() const noexcept { return m_bitsDepth; };
        inline uint32_t getDataSize() const noexcept { return m_dataSize; };
        inline std::shared_ptr<uint8_t[]> getData() const noexcept { return m_data; };
        inline std::shared_ptr<audio::Sound> getSound() const noexcept { return m_sound; };

    protected:
        int       m_channels = 0;
        uint32_t  m_sampleRate = 0;
        uint8_t   m_bitsDepth = 0;
        uint32_t  m_dataSize = 0;
        std::shared_ptr<uint8_t[]> m_data;
        std::shared_ptr<audio::Sound> m_sound;

        void make_sound_(const audio::mixer::eChannelGroup group);
    };
}
