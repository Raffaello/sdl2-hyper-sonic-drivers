#include <HyperSonicDrivers/utils/spdlog/Logger.hpp>

#include <spdlog/spdlog.h>
#include <format>

namespace HyperSonicDrivers::utils::spdlog
{

constexpr const char* cat2string(ILogger::eCategory cat)
{
    switch (cat)
    {
    default:
    case Logger::eCategory::Application:
        return "APP";
    case Logger::eCategory::System:
        return "SYS";
    case Logger::eCategory::Video:
        return "VID";
    case Logger::eCategory::Error:
        return "ERR";
    case Logger::eCategory::Audio:
        return "AUD";
    case Logger::eCategory::Input:
        return "INP";
    case Logger::eCategory::Render:
        return "REN";
    }
}

constexpr ::spdlog::level::level_enum level2spdlog(const Logger::eLevel level)
{
    switch (level)
    {
        using enum ILogger::eLevel;

    case Trace:
        return ::spdlog::level::trace;
    case Debug:
        return ::spdlog::level::debug;
    default:
    case Info:
        return ::spdlog::level::info;
    case Warning:
        return ::spdlog::level::warn;
    case Error:
        return ::spdlog::level::err;
    case Critical:
        return ::spdlog::level::critical;
    case Off:
        return ::spdlog::level::off;
    }
}

Logger::Logger()
{
    instance = this;
}

Logger::~Logger()
{
}

void Logger::setLevelAll(const eLevel level)
{
    // TODO: eventually split the category in multiple loggers
    m_level = level;
    ::spdlog::default_logger()->set_level(level2spdlog(level));
}

void Logger::setLevel(const eLevel level, const eCategory cat)
{
    // TODO: eventually split the category in multiple loggers
    m_level = level;
    ::spdlog::default_logger()->set_level(level2spdlog(level));
}

void Logger::trace(const std::string& str, const eCategory cat)
{
    ::spdlog::default_logger()->trace(std::format("{}: {}", cat2string(cat), str));
}

void Logger::debug(const std::string& str, const eCategory cat)
{
    ::spdlog::default_logger()->debug(std::format("{}: {}", cat2string(cat), str));
}

void Logger::info(const std::string& str, const eCategory cat)
{
    ::spdlog::default_logger()->info(std::format("{}: {}", cat2string(cat), str));
}

void Logger::warning(const std::string& str, const eCategory cat)
{
    ::spdlog::default_logger()->warn(std::format("{}: {}", cat2string(cat), str));
}

void Logger::error(const std::string& str, const eCategory cat)
{
    ::spdlog::default_logger()->error(std::format("{}: {}", cat2string(cat), str));
}

void Logger::critical(const std::string& str, const eCategory cat)
{
    ::spdlog::default_logger()->critical(std::format("{}: {}", cat2string(cat), str));
}

void Logger::enable()
{
    setLevel(m_level);
}

void Logger::disable()
{
    setLevel(eLevel::Off);
}

}    // namespace HyperSonicDrivers::utils::spdlog
