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
        [[deprecated("use other constructor that will create internally")]] explicit Adlib(
            const std::shared_ptr<hardware::opl::OPL>& opl,
            const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank);

        explicit Adlib(
            const hardware::opl::OplEmulator emuType,
            const std::shared_ptr<audio::IMixer>& mixer,
            const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank);

        [[deprecated("this is redundant: it has a default argument for 'emuType'")]] explicit Adlib(
            const std::shared_ptr<audio::IMixer>& mixer,
            const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank);

        ~Adlib() override = default;

        //void loadBankOP2();
    };
}
