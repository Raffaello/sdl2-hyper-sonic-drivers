#pragma once

#include <memory>
#include <HyperSonicDrivers/audio/opl/banks/OP2Bank.hpp>
#include <HyperSonicDrivers/devices/midi/MidiOpl.hpp>
#include <HyperSonicDrivers/hardware/opl/OplType.hpp>
#include <HyperSonicDrivers/hardware/opl/OplEmulator.hpp>
#include <HyperSonicDrivers/audio/mixer/ChannelGroup.hpp>

namespace HyperSonicDrivers::devices::midi
{
    class MidiOplDeviceMock : public MidiOpl
    {
    public:
        explicit MidiOplDeviceMock(const hardware::opl::OplType type,
            const hardware::opl::OplEmulator emuType,
            const std::shared_ptr<audio::IMixer>& mixer,
            const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank)
            : MidiOpl(type, emuType, mixer, op2Bank, audio::mixer::eChannelGroup::Plain, 255, 0) {}
        ~MidiOplDeviceMock() override = default;
    };
}
