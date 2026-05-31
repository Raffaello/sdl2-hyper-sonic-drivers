#include <HyperSonicDrivers/utils/Logger.hpp>

#include <format>
#include <iostream>

namespace HyperSonicDrivers::utils
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

Logger::Logger()
{
    instance = this;
}

Logger::~Logger()
{
    std::cerr.flush();
}

void Logger::setLevelAll(const eLevel level)
{
    // TODO: eventually split the category in multiple loggers
    m_level = level;
}

void Logger::setLevel(const eLevel level, const eCategory cat)
{
    // TODO: eventually split the category in multiple loggers
    m_level = level;
}

void Logger::trace(const std::string& str, const eCategory cat)
{
    if (m_level >= eLevel::Trace)
        std::cerr << std::format("trace: {}: {}\n", cat2string(cat), str);
}

void Logger::debug(const std::string& str, const eCategory cat)
{
    if (m_level >= eLevel::Debug)
        std::cerr << std::format("debug: {}: {}\n", cat2string(cat), str);
}

void Logger::info(const std::string& str, const eCategory cat)
{
    if (m_level >= eLevel::Info)
        std::cerr << std::format("info: {}: {}\n", cat2string(cat), str);
}

void Logger::warning(const std::string& str, const eCategory cat)
{
    if (m_level >= eLevel::Warning)
        std::cerr << std::format("warn: {}: {}\n", cat2string(cat), str);
}

void Logger::error(const std::string& str, const eCategory cat)
{
    if (m_level >= eLevel::Error)
        std::cerr << std::format("error: {}: {}\n", cat2string(cat), str);
}

void Logger::critical(const std::string& str, const eCategory cat)
{
    if (m_level >= eLevel::Trace)
        std::cerr << std::format("critical: {}: {}\n", cat2string(cat), str);
}

void Logger::enable()
{
    setLevel(m_level);
}

void Logger::disable()
{
    setLevel(eLevel::Off);
}

}    // namespace HyperSonicDrivers::utils
