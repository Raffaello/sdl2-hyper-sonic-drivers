#include <HyperSonicDrivers/hardware/IHardware.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>

namespace HyperSonicDrivers::hardware
{
    IHardware::IHardware(const std::shared_ptr<audio::IMixer>& mixer) :
        m_mixer(mixer)
    {
        if (m_mixer == nullptr)
        {
            utils::throwLogC<std::runtime_error>("mixer is null");
        }
    }

    void IHardware::start(
        const std::shared_ptr<TimerCallBack>& callback,
        const audio::mixer::eChannelGroup group,
        const uint8_t volume, const uint8_t pan,
        const int timerFrequency)
    {
        start_(callback, group, volume, pan, timerFrequency);
    }

    void IHardware::stop()
    {
        stopCallbacks();
        m_callback.reset();
    }

    void IHardware::start_(
        const std::shared_ptr<TimerCallBack>& callback,
        const audio::mixer::eChannelGroup group,
        const uint8_t volume, const uint8_t pan,
        const int timerFrequency)
    {
        m_callback = callback;
        startCallbacks(group, volume, pan, timerFrequency);
    }
    void IHardware::callCallback()
    {
        if (m_callback != nullptr)
            (*m_callback)();
    }
}
