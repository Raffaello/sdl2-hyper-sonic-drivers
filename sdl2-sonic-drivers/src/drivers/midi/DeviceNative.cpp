#include <drivers/midi/DeviceNative.hpp>
#include <vector>
#include <cassert>

namespace drivers
{
    namespace midi
    {
        DeviceNative::DeviceNative(const int port) : Device(),
            _midiout(std::make_shared<RtMidiOut>())
        {
            // default open port 0
            _midiout->openPort(port);
        }

        inline void DeviceNative::sendEvent(const audio::midi::MIDIEvent& e) noexcept
        {
            const int size = e.data.size() + 1;
            _m[0] = e.type.val;
            _m[1] = e.data[0];
            if (size == 3)
                _m[2] = e.data[1];

            _midiout->sendMessage(_m.data(), size);
        }

        inline void DeviceNative::sendMessage(const uint8_t msg[], const uint8_t size) noexcept
        {
            assert(size >= 2 && size <= 3);
            _midiout->sendMessage(msg, size);
        }
    }
}
