#pragma once

#include <string>
#include <format>
#include <HyperSonicDrivers/hardware/opl/OplEmulator.hpp>

namespace std
{
    template<>
    struct formatter<HyperSonicDrivers::hardware::opl::OplEmulator> : formatter<string_view>
    {
        template<typename FormatContext>
        auto format(HyperSonicDrivers::hardware::opl::OplEmulator emu, FormatContext& fc) const
        {
            using HyperSonicDrivers::hardware::opl::OplEmulator;
            string str;

            switch (emu)
            {
            case OplEmulator::AUTO:
                str = "AUTO";
                break;
            case OplEmulator::MAME:
                str = "MAME";
                break;
            case OplEmulator::DOS_BOX:
                str = "DOS_BOX";
                break;
            case OplEmulator::NUKED:
                str = "NUKED";
                break;
            case OplEmulator::WOODY:
                str = "WOODY";
                break;
            }

            return formatter<std::string_view>::format(str, fc);
        }
    };
}
