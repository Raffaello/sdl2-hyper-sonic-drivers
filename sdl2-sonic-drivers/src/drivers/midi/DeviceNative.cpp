#include <drivers/midi/DeviceNative.hpp>
#include <vector>
#include <cstdint>
#include <array>

namespace drivers
{
    namespace midi
    {
        DeviceNative::DeviceNative(const int port) : Device(), _midiout(std::make_shared<RtMidiOut>())
        {
            // default open port 0
            _midiout->openPort(port);
        }

        void DeviceNative::sendEvent(const audio::midi::MIDIEvent& e) const noexcept
        {
            std::array<uint8_t, 3> m = { e.type.val, e.data[0] };
            if(e.data.size() == 2)
                m[2] = e.data[1];

            _midiout->sendMessage(m.data(), e.data.size() + 1);
        }
    }
}
