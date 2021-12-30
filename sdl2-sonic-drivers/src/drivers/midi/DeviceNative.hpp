#pragma once

#include <drivers/midi/Device.hpp>
#include <RtMidi.h>
#include <memory>
#include <cstdint>
#include <array>


namespace drivers
{
    namespace midi
    {
        class DeviceNative : public Device
        {
        public:
            DeviceNative(const int port = 0);
            ~DeviceNative() = default;

            inline void sendEvent(const audio::midi::MIDIEvent& e) noexcept override;
            inline void sendMessage(const uint8_t msg[], const uint8_t size) noexcept override;

            std::shared_ptr<RtMidiOut> _midiout;
        private:
            std::array<uint8_t, 3> _m = {};
        };

    }
}
