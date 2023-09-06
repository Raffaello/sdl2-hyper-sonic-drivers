#pragma once

#include <HyperSonicDrivers/utils/ILogger.hpp>

namespace HyperSonicDrivers::utils::sdl2
{
    /**
    * default logger
    **/
    class Logger : public ILogger
    {
    public:
        Logger();
        virtual ~Logger();

        void setLevelAll(const eLevel level) override;
        void setLevel(const eLevel level, const eCategory cat = eCategory::Application) override;

        void trace(const std::string& str, const eCategory cat) override;
        void debug(const std::string& str, const eCategory cat = eCategory::Application) override;
        void info(const std::string& str, const eCategory cat = eCategory::Application) override;
        void warning(const std::string& str, const eCategory cat = eCategory::Application) override;
        void error(const std::string& str, const eCategory cat = eCategory::Application) override;
        void critical(const std::string& str, const eCategory cat = eCategory::Application) override;

        void enable() override;
        void disable() override;
    private:
    };
}
