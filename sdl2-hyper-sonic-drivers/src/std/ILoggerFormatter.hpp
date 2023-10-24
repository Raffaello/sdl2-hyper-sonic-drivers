#pragma once

#include <string>
#include <format>
#include <HyperSonicDrivers/utils/ILogger.hpp>

namespace std
{
    template<>
    struct formatter<HyperSonicDrivers::utils::ILogger::eLevel> : formatter<string_view>
    {
        template<typename FormatContext>
        auto format(HyperSonicDrivers::utils::ILogger::eLevel level, FormatContext& fc) const
        {
            string str;

            switch (level)
            {
                using enum HyperSonicDrivers::utils::ILogger::eLevel;

            case Trace:
                str = "Trace";
                break;
            case Debug:
                str = "Debug";
                break;
            case Info:
                str = "Info";
                break;
            case Warning:
                str = "Warning";
                break;
            case Error:
                str = "Error";
                break;
            case Critical:
                str = "Critical";
                break;
            case Off:
                str = "Off";
                break;
            }

            return formatter<std::string_view>::format(str, fc);
        }
    };
}
