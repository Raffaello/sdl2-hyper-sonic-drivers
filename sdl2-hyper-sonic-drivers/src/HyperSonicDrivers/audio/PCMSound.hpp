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
    class PCMSound final
    {
    public:
        PCMSound(PCMSound&) = delete;
        PCMSound(PCMSound&&) = delete;
        PCMSound& operator=(PCMSound&) = delete;
        PCMSound(
            const mixer::eChannelGroup group,
            const bool isStereo,
            const uint32_t freq,
            const uint32_t dataSize,
            const std::shared_ptr<int16_t[]> &data
        );

        const mixer::eChannelGroup group;
        const bool stereo;
        const uint32_t freq;
        uint32_t dataSize;
        std::shared_ptr<int16_t[]> data;
    };
}
