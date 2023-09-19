#include <HyperSonicDrivers/devices/midi/MidiSbPro2.hpp>

namespace HyperSonicDrivers::devices::midi
{
    MidiSbPro2::MidiSbPro2(
        const std::shared_ptr<audio::IMixer>& mixer,
        const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank,
        const audio::mixer::eChannelGroup group,
        const hardware::opl::OplEmulator emulator,
        const uint8_t volume,
        const uint8_t pan)
        : MidiOpl(hardware::opl::OplType::OPL3, emulator, mixer, op2Bank, group, volume, pan)
    {
    }
}
