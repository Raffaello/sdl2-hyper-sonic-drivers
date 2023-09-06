#include <HyperSonicDrivers/utils/ILogger.hpp>
#include <HyperSonicDrivers/utils/sdl2/Logger.hpp> // this one for sure exists for now as everything is built around SDL2,
// so default logger

namespace HyperSonicDrivers::utils
{
    ILogger::~ILogger()
    {
    }

    void ILogger::log(const eLevel level, const std::string& str, const eCategory cat)
    {
        switch (level)
        {
        case eLevel::Trace:
            trace(str, cat);
            break;
        case eLevel::Debug:
            debug(str, cat);
            break;
        case eLevel::Info:
            info(str, cat);
            break;
        case eLevel::Warning:
            warning(str, cat);
            break;
        case eLevel::Error:
            error(str, cat);
            break;
        default:
        case eLevel::Critical:
            critical(str, cat);
            break;
        case eLevel::Off:
            break;
        }
    }

    static sdl2::Logger g_default_sdl2_logger = sdl2::Logger();

    ILogger* ILogger::instance = &g_default_sdl2_logger;
}
