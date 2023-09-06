#include <format>
#include <HyperSonicDrivers/utils/sdl2/Logger.hpp>
#include <SDL2/SDL_log.h>
#include <SDL2/SDL_error.h>


namespace HyperSonicDrivers::utils::sdl2
{
    constexpr SDL_LogCategory cat2sdl(const Logger::eCategory cat)
    {
        switch (cat)
        {
        default:
        case Logger::eCategory::Application:
            return SDL_LOG_CATEGORY_APPLICATION;
        case Logger::eCategory::System:
            return SDL_LOG_CATEGORY_SYSTEM;
        case Logger::eCategory::Video:
            return SDL_LOG_CATEGORY_VIDEO;
        case Logger::eCategory::Error:
            return SDL_LOG_CATEGORY_ERROR;
        case Logger::eCategory::Audio:
            return SDL_LOG_CATEGORY_AUDIO;
        case Logger::eCategory::Input:
            return SDL_LOG_CATEGORY_INPUT;
        case Logger::eCategory::Render:
            return SDL_LOG_CATEGORY_RENDER;
        }
    }

    constexpr SDL_LogPriority level2sdl(const Logger::eLevel level)
    {
        switch(level)
        {
            using enum ILogger::eLevel;

        case Trace:
            return SDL_LOG_PRIORITY_VERBOSE;
        case Debug:
            return SDL_LOG_PRIORITY_DEBUG;
        default:
        case Info:
            return SDL_LOG_PRIORITY_INFO;
        case Warning:
            return SDL_LOG_PRIORITY_WARN;
        case Error:
            return SDL_LOG_PRIORITY_ERROR;
        case Critical:
            return SDL_LOG_PRIORITY_CRITICAL;
        case Off:
            // not supported
            throw std::runtime_error("not supported");
        }
    }

    static SDL_LogOutputFunction default_log_output_function = nullptr;
    static void* default_log_output_function_userdata = nullptr;

    static void log_output(void* userdata, int category, SDL_LogPriority priority, const char* msg)
    {
        std::string p;
        std::string c;
        FILE* s = nullptr;

        switch (category)
        {
        case SDL_LOG_CATEGORY_SYSTEM:
            c = "SYSTEM";
            break;
        case SDL_LOG_CATEGORY_APPLICATION:
            c = " * APP";
            break;
        case SDL_LOG_CATEGORY_ERROR:
            c = "ERROR!";
            break;
        case SDL_LOG_CATEGORY_VIDEO:
            c = "*VIDEO";
            break;
        case SDL_LOG_CATEGORY_RENDER:
            c = "RENDER";
            break;
        case SDL_LOG_CATEGORY_AUDIO:
            c = "_AUDIO";
            break;
        case SDL_LOG_CATEGORY_INPUT:
            c = "_INPUT";
            break;
        default:
            c = "_????_";
            break;
        }

        switch (priority)
        {
        case SDL_LogPriority::SDL_LOG_PRIORITY_DEBUG:
            p = "DEBUG";
            s = stdout;
            break;
        case SDL_LogPriority::SDL_LOG_PRIORITY_INFO:
            p = "_INFO";
            s = stdout;
            break;
        case SDL_LogPriority::SDL_LOG_PRIORITY_WARN:
            p = "_WARN";
            s = stdout;
            break;
        case SDL_LogPriority::SDL_LOG_PRIORITY_ERROR:
            p = "ERROR";
            s = stderr;
            break;
        case SDL_LogPriority::SDL_LOG_PRIORITY_CRITICAL:
            p = "CRTCL";
            s = stderr;
            break;
        case SDL_LogPriority::SDL_LOG_PRIORITY_VERBOSE:
            p = "VERBS";
            s = stdout;
            break;
        default:
            p = "???  ";
            s = stderr;
            break;
        }

        //printf("[%s.%s] %s\n", c.c_str(), p.c_str(), msg);
        fprintf(s, "[%s.%s] %s\n", c.c_str(), p.c_str(), msg);
    }

    Logger::Logger() : ILogger()
    {
        SDL_LogGetOutputFunction(&default_log_output_function, &default_log_output_function_userdata);

//#ifdef _DEBUG
//        setLevelAll(Logger::eLevel::Debug);
//        debug("Application is on Debug level logging.");
//#endif
        instance = this;
    }

    Logger::~Logger()
    {
        flushall();
    }

    void Logger::setLevelAll(const eLevel level)
    {
        SDL_LogSetAllPriority(level2sdl(level));
    }
    void Logger::setLevel(const eLevel level, const eCategory cat)
    {
        SDL_LogSetPriority(cat2sdl(cat), level2sdl(level));
    }
  
    void Logger::trace(const std::string& str, const eCategory cat)
    {
        SDL_LogVerbose(cat2sdl(cat), str.c_str());
    }

    void Logger::debug(const std::string& str, const eCategory cat)
    {
        SDL_LogDebug(cat2sdl(cat), str.c_str());
    }

    void Logger::info(const std::string& str, const eCategory cat)
    {
        SDL_LogInfo(cat2sdl(cat), str.c_str());
    }

    void Logger::warning(const std::string& str, const eCategory cat)
    {
        SDL_LogWarn(cat2sdl(cat), str.c_str());
    }

    void Logger::error(const std::string& str, const eCategory cat)
    {
        SDL_LogError(cat2sdl(cat), "%s - %s", str.c_str(), SDL_GetError());
        SDL_ClearError();
    }

    void Logger::critical(const std::string& str, const eCategory cat)
    {
        SDL_LogCritical(cat2sdl(cat), "%s - %s", str.c_str(), SDL_GetError());
        SDL_ClearError();
    }

    void Logger::enable()
    {
        // TODO: not sure make sense to override the default SDL2 setting in a library...
        //SDL_LogSetOutputFunction(&log_output, nullptr);

        SDL_LogSetOutputFunction(default_log_output_function, default_log_output_function_userdata);
    }

    void Logger::disable()
    {
        SDL_LogSetOutputFunction(nullptr, nullptr);
    }
}
