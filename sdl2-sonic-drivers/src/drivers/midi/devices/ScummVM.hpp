#pragma once

#include <drivers/midi/Device.hpp>
#include <drivers/midi/scummvm/adlib.h>
#include <memory>
#include <cstdint>
#include <hardware/opl/OPL.hpp>


namespace drivers
{
    namespace midi
    {
        namespace devices
        {
            /**
             * @brief This is using OS Midi devices
             * It is outside the internal Mixer at the moment.
             * It wasn't neither planned to use it, but it could
             * be useful to have it.
            */
            class ScummVM : public Device
            {
            public:
                explicit ScummVM(std::shared_ptr<hardware::opl::OPL> opl);
                ~ScummVM();

                inline void sendEvent(const audio::midi::MIDIEvent& e) const noexcept override;
                inline void sendMessage(const uint8_t msg[], const uint8_t size) const noexcept override;

            private:
                std::shared_ptr<MidiDriver_ADLIB> _adlib;
            };
        }
    }
}
