#pragma once

#include <string>
#include <format>
#include <HyperSonicDrivers/hardware/opl/OplType.hpp>

namespace std
{
    template<>
    struct formatter<HyperSonicDrivers::hardware::opl::OplType> : formatter<string_view>
    {
        template<typename FormatContext>
        auto format(HyperSonicDrivers::hardware::opl::OplType t, FormatContext& fc) const
        {
            using HyperSonicDrivers::hardware::opl::OplType;
            string str;

            switch (t)
            {
            case OplType::OPL2:
                str = "OPL2";
                break;
            case OplType::DUAL_OPL2:
                str = "DUAL_OPL2";
                break;
            case OplType::OPL3:
                str = "OPL3";
                break;
            }

            return formatter<std::string_view>::format(str, fc);
        }
    };
}
