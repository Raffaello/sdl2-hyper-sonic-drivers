#pragma once

#include <cstdint>
#include <HyperSonicDrivers/audio/mixer/consts.hpp>

namespace HyperSonicDrivers::audio::mixer
{
    enum class eChannelGroup
    {
        Plain = 0,
        Music = 1,
        Sfx = 2,
        Speech = 3,
        Unknown = 4,
    };

    constexpr int eChannelGroup_size = 5;

    constexpr size_t group2i(const mixer::eChannelGroup group)
    {
        return static_cast<size_t>(group);
    }

    typedef struct channelGroupSettings_t
    {
        uint8_t volume = Channel_max_volume;
        int8_t  pan = 0; // TODO: not implemented yet
        bool mute = false; 
        bool reverseStereo = false; // TODO: not implemented yet
    } channelGroupSettings_t;
}
