#pragma once

#include <audio/midi/MIDIEvent.hpp>
#include <cstdint>

namespace drivers
{
    namespace midi
    {
        class Device
        {
        public:
            Device() = default;
            ~Device() = default;

            virtual inline void sendEvent(const audio::midi::MIDIEvent& e) const noexcept = 0;
            virtual inline void sendMessage(const uint8_t msg[], const uint8_t size) const noexcept = 0;
        };
    }
}
