#include <HyperSonicDrivers/drivers/midi/devices/Adlib.hpp>

namespace HyperSonicDrivers::drivers::midi::devices
{
    Adlib::Adlib(
        const std::shared_ptr<audio::IMixer>& mixer,
        const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank,
        const hardware::opl::OplEmulator emulator)
        : Opl(hardware::opl::OplType::OPL2, emulator, mixer, op2Bank)
    {
    }
}
