#pragma once

#include <string>
#include <format>
#include <source_location>
#include <functional>

namespace HyperSonicDrivers::utils
{
    class ILogger
    {
    public:
        static ILogger* instance;

        enum class eLevel
        {
            Trace,
            Debug,
            Info,
            Warning,
            Error,
            Critical,
            Off
        };

        enum class eCategory
        {
            Application,
            System,
            Error,
            Video,
            Audio,
            Input,
            Render
        };

        ILogger(ILogger&) = delete;
        ILogger(ILogger&&) = delete;
        ILogger& operator=(ILogger&) = delete;

        ILogger() = default;
        virtual ~ILogger() = default;

        virtual void setLevelAll(const eLevel level) = 0;
        virtual void setLevel(const eLevel level, const eCategory cat) = 0;

        virtual void trace(const std::string& str, const eCategory cat) = 0;
        virtual void debug(const std::string& str, const eCategory cat) = 0;
        virtual void info(const std::string& str, const eCategory cat) = 0;
        virtual void warning(const std::string& str, const eCategory cat) = 0;
        virtual void error(const std::string& str, const eCategory cat) = 0;
        virtual void critical(const std::string& str, const eCategory cat) = 0;

        void log(const eLevel level, const std::string& str, const eCategory cat);

        virtual void enable() = 0;
        virtual void disable() = 0;

    protected:
        // TODO need to add a level for each category?
        //eLevel m_level = eLevel::INFO;
    };

    constexpr auto logMsg_(const std::source_location& loc, const std::string& msg)
    {
        return std::format("[{}] {}", loc.function_name(), msg).c_str();
    }

    constexpr void logT(const std::string& msg,
        const ILogger::eCategory cat = ILogger::eCategory::Audio,
        const std::source_location& loc =
        std::source_location::current())
    {
        ILogger::instance->trace(logMsg_(loc, msg), cat);
    }

    constexpr void logD(const std::string& msg,
        const ILogger::eCategory cat = ILogger::eCategory::Audio,
        const std::source_location& loc =
        std::source_location::current())
    {
        ILogger::instance->debug(logMsg_(loc, msg), cat);
    }

    constexpr void logI(const std::string& msg,
        const ILogger::eCategory cat = ILogger::eCategory::Audio,
        const std::source_location& loc =
        std::source_location::current())
    {
        ILogger::instance->info(logMsg_(loc, msg), cat);
    }

    constexpr void logW(const std::string& msg,
        const ILogger::eCategory cat = ILogger::eCategory::Audio,
        const std::source_location& loc =
        std::source_location::current())
    {
        ILogger::instance->warning(logMsg_(loc, msg), cat);
    }


    constexpr void logE(const std::string& msg,
        const ILogger::eCategory cat = ILogger::eCategory::Audio,
        const std::source_location& loc =
        std::source_location::current())
    {
        ILogger::instance->error(logMsg_(loc, msg), cat);
    }

    constexpr void logC(const std::string& msg,
        const ILogger::eCategory cat = ILogger::eCategory::Audio,
        const std::source_location& loc =
        std::source_location::current())
    {
        ILogger::instance->critical(logMsg_(loc, msg), cat);
    }

    template<class e>
    constexpr void throwLogE(
        const std::string& msg,
        const ILogger::eCategory cat = ILogger::eCategory::Audio,
        const std::source_location& loc =
        std::source_location::current())
    {
        logE(msg, cat, loc);
        throw e(msg);
    }

    template<class e>
    constexpr void throwLogC(
        const std::string& msg,
        const ILogger::eCategory cat = ILogger::eCategory::Audio,
        const std::source_location& loc =
        std::source_location::current())
    {
        logC(msg, cat, loc);
        throw e(msg);
    }
}
