#include <HyperSonicDrivers/drivers/midi/devices/SbPro.hpp>

namespace HyperSonicDrivers::drivers::midi::devices
{
    SbPro::SbPro(
        const std::shared_ptr<audio::IMixer>& mixer,
        const audio::mixer::eChannelGroup group,
        const uint8_t volume,
        const uint8_t pan,
        const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank,
        const hardware::opl::OplEmulator emulator)
        : Opl(hardware::opl::OplType::DUAL_OPL2, emulator, mixer, op2Bank, group, volume, pan)
    {
    }
}
