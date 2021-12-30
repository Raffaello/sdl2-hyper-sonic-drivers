#pragma once

#include <audio/midi/MIDIEvent.hpp>

namespace drivers
{
    namespace midi
    {
        class Device
        {
        public:
            Device() = default;
            ~Device() = default;

            virtual void sendEvent(const audio::midi::MIDIEvent& e) const noexcept = 0;
        };
    }
}
