#pragma once

#include <drivers/midi/Device.hpp>
#include <rtmidi/RtMidi.h>
#include <memory>
#include <cstdint>


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
            class Native : public Device
            {
            public:
                explicit Native(const int port = 0);
                virtual ~Native() = default;

                inline void sendEvent(const audio::midi::MIDIEvent& e) const noexcept override;
                inline void sendMessage(const uint8_t msg[], const uint8_t size) const noexcept override;
                inline void sendSysEx(const audio::midi::MIDIEvent& e) const noexcept override;

            private:
                std::shared_ptr<RtMidiOut> _midiout;
            };
        }
    }
}
