#include <HyperSonicDrivers/utils/ILogger.hpp>
#if HAS_SDL3
#include <HyperSonicDrivers/utils/sdl3/Logger.hpp>    // this one for sure exists for now as everything is built around SDL2,
#else
#include <HyperSonicDrivers/utils/sdl2/Logger.hpp>    // this one for sure exists for now as everything is built around SDL2,
#endif
// so default logger

namespace HyperSonicDrivers::utils
{
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

#if HAS_SDL3
static sdl3::Logger g_default_sdl3_logger = sdl3::Logger();
ILogger*            ILogger::instance     = &g_default_sdl3_logger;

#else
static sdl2::Logger g_default_sdl2_logger = sdl2::Logger();

ILogger* ILogger::instance = &g_default_sdl2_logger;
#endif

}    // namespace HyperSonicDrivers::utils
