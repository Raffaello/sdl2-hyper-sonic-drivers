#pragma once

#include <string>
#include <format>
#include <HyperSonicDrivers/devices/types.hpp>

namespace std
{
    template<>
    struct formatter<HyperSonicDrivers::devices::eDeviceName> : formatter<string_view>
    {
        template<typename FormatContext>
        auto format(HyperSonicDrivers::devices::eDeviceName device_name, FormatContext& fc) const
        {
            string str;

            switch (device_name)
            {
                using enum HyperSonicDrivers::devices::eDeviceName;

            case Adlib:
                str = "Adlib";
                break;
            case SbPro:
                str = "SbPro";
                break;
            case SbPro2:
                str = "SbPro2";
                break;
            case Mt32:
                str = "MT32";
                break;
            }

            return formatter<std::string_view>::format(str, fc);
        }
    };
}
