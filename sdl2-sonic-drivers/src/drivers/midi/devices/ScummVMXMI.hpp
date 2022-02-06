#pragma once

#include <memory>
#include <cstdint>
#include <drivers/midi/Device.hpp>
#include <drivers/midi/scummvm/miles/MidiDriver_Miles_AdLib.hpp>
#include <hardware/opl/OPL.hpp>


namespace drivers
{
    namespace midi
    {
        namespace devices
        {
            /**
             * @brief Wrapper around ScummVM MidiDriver
             * At the moment support only OPL
             * Better rename to OPL?
            */
            class ScummVMXMI : public Device
            {
            public:
                explicit ScummVMXMI(std::shared_ptr<hardware::opl::OPL> opl, const bool opl3mode);
                ~ScummVMXMI();

                inline void sendEvent(const audio::midi::MIDIEvent& e) const noexcept override;
                inline void sendMessage(const uint8_t msg[], const uint8_t size) const noexcept override;

            private:
                std::shared_ptr<drivers::midi::scummvm::miles::MidiDriver_Miles_AdLib> _adlib;
            };
        }
    }
}
