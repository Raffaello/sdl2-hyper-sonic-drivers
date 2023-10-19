#pragma once

#include <string>
#include <format>
#include <HyperSonicDrivers/audio/mixer/ChannelGroup.hpp>

namespace std
{
    template<>
    struct formatter<HyperSonicDrivers::audio::mixer::eChannelGroup> : formatter<string_view>
    {
        template<typename FormatContext>
        auto format(HyperSonicDrivers::audio::mixer::eChannelGroup ch_group, FormatContext& fc) const
        {
            string str;

            switch (ch_group)
            {
                using enum HyperSonicDrivers::audio::mixers::eChannelGroup;

            case Plain:
                str = "Plain";
                break;
            case Music:
                str = "Music";
                break;
            case Sfx:
                str = "Sfx";
                break;
            case Speech:
                str = "Speech";
                break;
            case Unknown:
                str = "Unknown";
                break;
            }

            return formatter<std::string_view>::format(str, fc);
        }
    };
}
