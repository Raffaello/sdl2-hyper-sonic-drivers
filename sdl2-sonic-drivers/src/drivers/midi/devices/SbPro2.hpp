#pragma once

#include <memory>
#include <drivers/midi/devices/Opl.hpp>
#include <hardware/opl/OPL.hpp>
#include <audio/opl/banks/OP2Bank.hpp>

namespace drivers
{
    namespace midi
    {
        namespace devices
        {
            class SbPro2 : public Opl
            {
            public:
                // TODO review the constructor and use a load bank instead..
                // TODO can create its own OPL2 chip, just need the OPL type (DOSBOX,MAME,etc..)
                SbPro2(const std::shared_ptr<hardware::opl::OPL>& opl, const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank);
                ~SbPro2() = default;

                //void loadBankOP2();
            };
        }
    }
}
