#include <HyperSonicDrivers/devices/IDevice.hpp>

namespace HyperSonicDrivers::devices
{
    IDevice::IDevice(const std::shared_ptr<audio::IMixer>& mixer) :
        m_mixer(mixer)
    {
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
