#include <HyperSonicDrivers/devices/midi/MidiSbPro.hpp>

namespace HyperSonicDrivers::devices::midi
{
    MidiSbPro::MidiSbPro(
        const std::shared_ptr<audio::IMixer>& mixer,
        const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank,
        const audio::mixer::eChannelGroup group,
        const hardware::opl::OplEmulator emulator,
        const uint8_t volume,
        const uint8_t pan)
        : MidiOpl(hardware::opl::OplType::DUAL_OPL2, emulator, mixer, op2Bank, group, volume, pan)
    {
    }

    MidiSbPro::MidiSbPro(
        const std::shared_ptr<audio::IMixer>& mixer,
        const audio::mixer::eChannelGroup group,
        const hardware::opl::OplEmulator emulator,
        const uint8_t volume,
        const uint8_t pan)
        : MidiOpl(hardware::opl::OplType::DUAL_OPL2, emulator, mixer, group, volume, pan)
    {
    }
}
