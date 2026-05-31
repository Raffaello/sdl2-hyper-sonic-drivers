#include <HyperSonicDrivers/utils/ILogger.hpp>

#if HAS_SPDLOG
#include <HyperSonicDrivers/utils/spdlog/Logger.hpp>    // TODO if USE_SPDLOG
#define LOGGER spdlog::Logger
#elif HAS_SDL3
#include <HyperSonicDrivers/utils/sdl3/Logger.hpp>
#define LOGGER sdl3::Logger
#elif HAS_SDL2
#include <HyperSonicDrivers/utils/sdl2/Logger.hpp>
#define LOGGER sdl2::Logger
#else
#include <HyperSonicDrivers/utils/Logger.hpp>
#define LOGGER Logger
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

// #if HAS_SDL3
// static sdl3::Logger g_default_sdl3_logger = sdl3::Logger();
// ILogger*            ILogger::instance     = &g_default_sdl3_logger;

// #elif HAS_SDL2
// static sdl2::Logger g_default_sdl2_logger = sdl2::Logger();
// ILogger*            ILogger::instance     = &g_default_sdl2_logger;

// #else
static auto g_default_logger  = LOGGER();
ILogger*    ILogger::instance = &g_default_logger;
// #endif

}    // namespace HyperSonicDrivers::utils
