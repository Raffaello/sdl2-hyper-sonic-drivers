#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <HyperSonicDrivers/audio/IMixer.hpp>
#include <HyperSonicDrivers/audio/mixer/ChannelGroup.hpp>

namespace HyperSonicDrivers::audio
{
    /**
    * 16 bits signed PCM sound
    * if the original data is not in this format must be converted first
    **/
    class Sound final
    {
    public:
        Sound(Sound&) = delete;
        Sound(Sound&&) = delete;
        Sound& operator=(Sound&) = delete;
        Sound(
            const mixer::eChannelGroup group,
            const bool isStereo,
            const uint32_t freq,
            const uint32_t dataSize,
            const std::shared_ptr<int16_t[]> &data
        );

        const mixer::eChannelGroup group;
        const bool stereo;
        const uint32_t freq;

        inline uint32_t dataSize() const noexcept { return m_dataSize; };
        inline std::shared_ptr<int16_t[]> data() const noexcept { return m_data; };
    private:
        uint32_t m_dataSize;
        std::shared_ptr<int16_t[]> m_data;
    };
}
