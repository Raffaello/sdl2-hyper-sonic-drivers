#pragma once

#include <drivers/midi/Device.hpp>
#include <RtMidi.h>
#include <memory>
#include <cstdint>


namespace drivers
{
    namespace midi
    {
        namespace devices
        {
            class Native : public Device
            {
            public:
                explicit Native(const int port = 0);
                ~Native() = default;

                inline void sendEvent(const audio::midi::MIDIEvent& e) const noexcept override;
                inline void sendMessage(const uint8_t msg[], const uint8_t size) const noexcept override;

            private:
                std::shared_ptr<RtMidiOut> _midiout;
            };
        }
    }
}
