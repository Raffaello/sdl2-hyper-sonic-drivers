#include <HyperSonicDrivers/devices/IDevice.hpp>

namespace HyperSonicDrivers::devices
{
    IDevice::IDevice(const std::shared_ptr<audio::IMixer>& mixer, const bool isOpl) :
        m_mixer(mixer), m_isOpl(isOpl)
    {
    }

    bool IDevice::acquire(drivers::IMusicDriver* owner)
    {
        if (!m_acquired)
        {
            if (!init())
                return false;

            m_acquired = true;
            m_owner = owner;
            return true;
        }

        return isOwned(owner);
    }

    bool IDevice::release(const drivers::IMusicDriver* owner)
    {
        if (isOwned(owner))
        {
            shutdown();
            m_acquired = false;
            return true;
        }

        return !isAcquired();
    }

    void IDevice::setVolume(const uint8_t volume)
    {
        auto ch = getChannelId();
        if(ch.has_value())
            m_mixer->setChannelVolume(ch.value(), volume);
    }

    void IDevice::setPan(const uint8_t pan)
    {
        auto ch = getChannelId();
        if (ch.has_value())
            m_mixer->setChannelPan(ch.value(), pan);
    }

    void IDevice::setVolumePan(const uint8_t volume, const uint8_t pan)
    {
        auto ch = getChannelId();
        if (ch.has_value())
            m_mixer->setChannelVolumePan(ch.value(), volume, pan);
    }
}
