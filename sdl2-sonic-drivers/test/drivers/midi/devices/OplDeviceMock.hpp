#pragma once

#include <memory>
#include <audio/opl/banks/OP2Bank.hpp>
#include <drivers/midi/devices/Opl.hpp>
#include <hardware/opl/OplType.hpp>
#include <hardware/opl/OplEmulator.hpp>
#include <audio/scummvm/Mixer.hpp>

namespace drivers::midi::devices
{
    class OplDeviceMock : public Opl
    {
    public:
        explicit OplDeviceMock(
            const std::shared_ptr<hardware::opl::OPL>& opl,
            const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank)
            : Opl(opl, op2Bank) {}
        explicit OplDeviceMock(const hardware::opl::OplType type,
            const hardware::opl::OplEmulator emuType,
            const std::shared_ptr<audio::scummvm::Mixer>& mixer,
            const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank)
            : Opl(type, emuType, mixer, op2Bank) {}
        ~OplDeviceMock() override = default;
    };
}
