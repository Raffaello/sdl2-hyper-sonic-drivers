#include <drivers/midi/devices/Native.hpp>
#include <array>
#include <cassert>

namespace drivers
{
    namespace midi
    {
        namespace devices
        {
            Native::Native(const int port) : Device(),
                _midiout(std::make_shared<RtMidiOut>())
            {
                // default open port 0
                _midiout->openPort(port);
            }

            void Native::sendEvent(const audio::midi::MIDIEvent& e) const noexcept
            {
                std::array<uint8_t, 3> m;
                const size_t size = e.data.size() + 1;
                m[0] = e.type.val;
                m[1] = e.data[0];
                if (size == 3)
                    m[2] = e.data[1];

                _midiout->sendMessage(m.data(), size);
            }

            void Native::sendMessage(const uint8_t msg[], const uint8_t size) const noexcept
            {
                assert(size >= 2 && size <= 3);
                _midiout->sendMessage(msg, size);
            }

            void Native::sendSysEx(const audio::midi::MIDIEvent& e) const noexcept
            {
                // TODO
            }

            void Native::pause() const noexcept
            {
                // TODO
            }

            void Native::resume() const noexcept
            {
                // TODO
            }
        }
    }
}
