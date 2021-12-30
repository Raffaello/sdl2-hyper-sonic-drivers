#pragma once

#include <drivers/midi/Device.hpp>
#include <RtMidi.h>
#include <memory>

namespace drivers
{
    namespace midi
    {
        class DeviceNative : public Device
        {
        public:
            DeviceNative(const int port = 0);
            ~DeviceNative() = default;

            void sendEvent(const audio::midi::MIDIEvent& e) const noexcept override;

        private:
            std::shared_ptr<RtMidiOut> _midiout;

        };

    }
}
