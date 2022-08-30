#pragma once

#include <drivers/midi/Device.hpp>
#include <memory>
#include <hardware/opl/OPL.hpp>
#include <drivers/midi/adlib/MidiDriver.hpp>
#include <audio/midi/MIDIEvent.hpp>
#include <audio/opl/banks/OP2Bank.h>

namespace drivers
{
    namespace midi
    {
        namespace devices
        {
            class Adlib : public Device
            {
            public:
                // TOOD review the constructor and use a load bank instead..
                Adlib(std::shared_ptr<hardware::opl::OPL> opl, std::shared_ptr<audio::opl::banks::OP2Bank> op2Bank);
                ~Adlib() = default;

                inline void sendEvent(const audio::midi::MIDIEvent& e) const noexcept override;
                inline void sendMessage(const uint8_t msg[], const uint8_t size) const noexcept override;
                inline void sendSysEx(const audio::midi::MIDIEvent& e) const noexcept override;
                
                //void loadBankOP2();
            private:
                std::shared_ptr<drivers::midi::adlib::MidiDriver> _adlib;
            };
        }
    }
}
