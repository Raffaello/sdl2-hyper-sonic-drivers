#include <HyperSonicDrivers/devices/Adlib.hpp>
#include <HyperSonicDrivers/hardware/opl/OplType.hpp>

namespace HyperSonicDrivers::devices
{
    using hardware::opl::OplType;

    Adlib::Adlib(
        const std::shared_ptr<audio::IMixer>& mixer,
        const hardware::opl::OplEmulator emulator,
        const uint8_t volume, const uint8_t pan) :
        Opl(mixer, emulator, OplType::OPL2, volume, pan)
    {
    }

    eDeviceName Adlib::getName() const noexcept
    {
        return eDeviceName::Adlib;
    }
}
