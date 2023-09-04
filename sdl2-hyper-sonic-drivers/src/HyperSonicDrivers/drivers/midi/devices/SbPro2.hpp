#pragma once

#include <memory>
#include <HyperSonicDrivers/audio/opl/banks/OP2Bank.hpp>
#include <HyperSonicDrivers/drivers/midi/devices/Opl.hpp>
#include <HyperSonicDrivers/hardware/opl/OPL.hpp>

namespace HyperSonicDrivers::drivers::midi::devices
{
    class SbPro2 : public Opl
    {
    public:
        /**
        * @deprecated
        */
        explicit SbPro2(const std::shared_ptr<hardware::opl::OPL>& opl, const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank);
        explicit SbPro2(const hardware::opl::OplEmulator emuType,
            const std::shared_ptr<audio::scummvm::Mixer>& mixer,
            const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank);
        explicit SbPro2(const std::shared_ptr<audio::scummvm::Mixer>& mixer,
            const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank);
        virtual ~SbPro2() = default;

        //void loadBankOP2();
    };
}
