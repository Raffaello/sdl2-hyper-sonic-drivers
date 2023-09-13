#pragma once

#include <memory>
#include <HyperSonicDrivers/audio/opl/banks/OP2Bank.hpp>
#include <HyperSonicDrivers/drivers/midi/devices/Opl.hpp>
#include <HyperSonicDrivers/hardware/opl/OplType.hpp>
#include <HyperSonicDrivers/hardware/opl/OplEmulator.hpp>
#include <HyperSonicDrivers/audio/mixer/ChannelGroup.hpp>

namespace HyperSonicDrivers::drivers::midi::devices
{
    class OplDeviceMock : public Opl
    {
    public:
        explicit OplDeviceMock(
            const std::shared_ptr<hardware::opl::OPL>& opl,
            const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank)
            : Opl(opl, op2Bank, audio::mixer::eChannelGroup::Plain, 255, 0) {}
        explicit OplDeviceMock(const hardware::opl::OplType type,
            const hardware::opl::OplEmulator emuType,
            const std::shared_ptr<audio::IMixer>& mixer,
            const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank)
            : Opl(type, emuType, mixer, op2Bank, audio::mixer::eChannelGroup::Plain, 255, 0) {}
        ~OplDeviceMock() override = default;
    };
}
