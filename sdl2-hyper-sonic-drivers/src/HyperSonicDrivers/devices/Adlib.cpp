#include <HyperSonicDrivers/devices/Adlib.hpp>
#include <HyperSonicDrivers/hardware/opl/OPLFactory.hpp>
#include <HyperSonicDrivers/hardware/opl/OplType.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>
#include <std/OplEmulatorFormatter.hpp>

namespace HyperSonicDrivers::devices
{
    using hardware::opl::OplType;

    Adlib::Adlib(
        const std::shared_ptr<audio::IMixer>& mixer,
        //const audio::mixer::eChannelGroup group,
        const hardware::opl::OplEmulator emulator,
        const uint8_t volume, const uint8_t pan) :
        Opl(mixer/*, group*/, emulator, OplType::OPL2, volume, pan)
    {
    }
}
