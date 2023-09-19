#pragma once

#include <memory>
#include <HyperSonicDrivers/audio/opl/banks/OP2Bank.hpp>
#include <HyperSonicDrivers/devices/midi/MidiOpl.hpp>
#include <HyperSonicDrivers/hardware/opl/OPL.hpp>

namespace HyperSonicDrivers::devices::midi
{
    class MidiSbPro2 : public MidiOpl
    {
    public:
        explicit MidiSbPro2(
            const std::shared_ptr<audio::IMixer>& mixer,
            const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank,
            const audio::mixer::eChannelGroup group,
            const hardware::opl::OplEmulator emulator = hardware::opl::OplEmulator::AUTO,
            const uint8_t volume = 255,
            const uint8_t pan = 0);

        ~MidiSbPro2() override = default;

        //void loadBankOP2();
    };
}
