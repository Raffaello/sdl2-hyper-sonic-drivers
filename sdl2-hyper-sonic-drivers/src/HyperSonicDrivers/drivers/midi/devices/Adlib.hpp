#pragma once

#include <memory>
#include <HyperSonicDrivers/audio/opl/banks/OP2Bank.hpp>
#include <HyperSonicDrivers/drivers/midi/devices/Opl.hpp>
#include <HyperSonicDrivers/hardware/opl/OPL.hpp>

namespace HyperSonicDrivers::drivers::midi::devices
{
    class Adlib : public Opl
    {
    public:
        explicit Adlib(
            const std::shared_ptr<audio::IMixer>& mixer,
            const audio::mixer::eChannelGroup group,
            const uint8_t volume,
            const uint8_t pan,
            const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank,
            const hardware::opl::OplEmulator emulator = hardware::opl::OplEmulator::AUTO);

        ~Adlib() override = default;

        //void loadBankOP2();
    };
}
