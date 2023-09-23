#include <HyperSonicDrivers/devices/SbPro2.hpp>
#include <HyperSonicDrivers/hardware/opl/OPLFactory.hpp>
#include <HyperSonicDrivers/hardware/opl/OplType.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>
#include <std/OplEmulatorFormatter.hpp>

namespace HyperSonicDrivers::devices
{
    using hardware::opl::OplType;

    SbPro2::SbPro2(
        const std::shared_ptr<audio::IMixer>& mixer,
        const hardware::opl::OplEmulator emulator,
        const uint8_t volume, const uint8_t pan) :
        Opl(mixer, emulator, OplType::OPL3, volume, pan)
    {
    }
}
