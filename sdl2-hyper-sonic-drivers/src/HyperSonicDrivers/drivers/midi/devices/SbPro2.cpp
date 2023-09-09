#include <HyperSonicDrivers/drivers/midi/devices/SbPro2.hpp>

namespace HyperSonicDrivers::drivers::midi::devices
{
    SbPro2::SbPro2(
        const std::shared_ptr<audio::IMixer>& mixer,
        const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank,
        const hardware::opl::OplEmulator emulator)
        : Opl(hardware::opl::OplType::OPL3, emulator, mixer, op2Bank)
    {
    }
}
