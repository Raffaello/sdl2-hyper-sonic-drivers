#pragma once

#include <memory>
#include <drivers/midi/Device.hpp>
#include <drivers/midi/opl/OplDriver.hpp>
#include <audio/opl/banks/OP2Bank.hpp>
#include <hardware/opl/OPL.hpp>

namespace drivers
{
    namespace midi
    {
        namespace devices
        {
            class Opl : public Device
            {
            public:
                void sendEvent(const audio::midi::MIDIEvent& e) const noexcept override;
                void sendMessage(const uint8_t msg[], const uint8_t size) const noexcept override;
                void sendSysEx(const audio::midi::MIDIEvent& e) const noexcept override;

                //void loadBankOP2();
            protected:
                // TODO review the constructor and use a load bank instead..
                // TODO can create its own OPL2 chip, just need the OPL type (DOSBOX,MAME,etc..)
                Opl(const std::shared_ptr<hardware::opl::OPL>& opl, const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank, const bool opl3_mode);
                virtual ~Opl() = default;

                std::shared_ptr<drivers::midi::opl::OplDriver> _opl;
            };
        }
    }
}
