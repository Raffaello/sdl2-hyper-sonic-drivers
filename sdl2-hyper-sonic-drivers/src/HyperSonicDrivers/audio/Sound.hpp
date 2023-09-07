#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <HyperSonicDrivers/audio/IMixer.hpp>
#include <HyperSonicDrivers/audio/mixer/ChannelGroup.hpp>

namespace HyperSonicDrivers::audio
{
    class Sound final
    {
    public:
        Sound(Sound&) = delete;
        Sound(Sound&&) = delete;
        Sound& operator=(Sound&) = delete;
        // TODO: convert sound to "mixer bits depts at the constructor level.
        //       use a unique_ptr for data instead of shared?
        Sound(
            const mixer::eChannelGroup group,
            const bool isStereo,
            const uint32_t freq,
            const uint8_t bitsDepth,
            //const uint32_t dataSize,
            const std::shared_ptr<std::vector<uint8_t>>& data
        );

        const mixer::eChannelGroup group;
        const bool stereo;
        const uint8_t bitsDepth;
        const uint32_t freq;
        //const uint32_t dataSize;
        const std::shared_ptr<std::vector<uint8_t>> data;
    };
}
