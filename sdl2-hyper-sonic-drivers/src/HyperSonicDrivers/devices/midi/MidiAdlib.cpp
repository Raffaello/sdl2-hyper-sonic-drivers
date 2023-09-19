#include <HyperSonicDrivers/devices/midi/MidiAdlib.hpp>

namespace HyperSonicDrivers::devices::midi
{
    MidiAdlib::MidiAdlib(
        const std::shared_ptr<audio::IMixer>& mixer,
        const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank,
        const audio::mixer::eChannelGroup group,
        const hardware::opl::OplEmulator emulator,
        const uint8_t volume,
        const uint8_t pan)
        : MidiOpl(hardware::opl::OplType::OPL2, emulator, mixer, op2Bank, group, volume, pan)
    {
    }
}
