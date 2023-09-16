#include <format>
#include <HyperSonicDrivers/hardware/opl/OPL.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>

namespace HyperSonicDrivers::hardware::opl
{
    using utils::logE;

    OPL::OPL(const OplType type) : type(type)
    {
    }

    void OPL::start(
        const std::shared_ptr<TimerCallBack>& callback,
        const audio::mixer::eChannelGroup group,
        const uint8_t volume,
        const uint8_t pan,
        const int timerFrequency)
    {
        m_callback = callback;
        startCallbacks(group, volume, pan, timerFrequency);
    }

    void OPL::stop()
    {
        stopCallbacks();
        m_callback.reset();
    }
}
