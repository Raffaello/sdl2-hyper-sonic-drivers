#include <HyperSonicDrivers/devices/SbPro.hpp>
#include <HyperSonicDrivers/hardware/opl/OplType.hpp>

namespace HyperSonicDrivers::devices
{
    using hardware::opl::OplType;

    SbPro::SbPro(
        const std::shared_ptr<audio::IMixer>& mixer,
        const hardware::opl::OplEmulator emulator,
        const uint8_t volume, const uint8_t pan) :
        Opl(mixer, emulator, OplType::DUAL_OPL2, volume, pan)
    {
    }
}
