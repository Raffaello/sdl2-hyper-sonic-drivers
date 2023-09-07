#include <format>
#include <HyperSonicDrivers/hardware/opl/OPL.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>

namespace HyperSonicDrivers::hardware::opl
{
    using utils::logE;

    // TODO: review to allow to have multiple OPL chips instead.
    static bool _hasInstance;

    OPL::OPL(const OplType type) : type(type)
    {
        if (_hasInstance)
        {
            logE("There are multiple OPL output instances running");
        }

        _hasInstance = true;
    }

    OPL::~OPL()
    {
        _hasInstance = false;
    }

    void OPL::start(const std::shared_ptr<TimerCallBack>& callback, int timerFrequency)
    {
        _callback = callback;
        startCallbacks(timerFrequency);
    }

    void OPL::stop()
    {
        stopCallbacks();
        _callback.reset();
    }
}
