#include <drivers/midi/devices/Adlib.hpp>
#include <cassert>
#include <array>

namespace drivers
{
    namespace midi
    {
        namespace devices
        {
            Adlib::Adlib(std::shared_ptr<hardware::opl::OPL> opl)
            {
                _adlib = std::make_shared<drivers::midi::adlib::MidiDriver>(opl);

            }

            inline void Adlib::sendEvent(const audio::midi::MIDIEvent& e) const noexcept
            {
                _adlib->send(e);
            }

            inline void Adlib::sendMessage(const uint8_t msg[], const uint8_t size) const noexcept
            {
                assert(size >= 2 && size <= 3);
                audio::midi::MIDIEvent e;
                
                e.type.val = msg[0];
                e.data.push_back(msg[1]);
                if (size == 3) {
                    e.data.push_back(msg[2]);
                }

                sendEvent(e);
            }

            inline void Adlib::sendSysEx(const audio::midi::MIDIEvent& e) const noexcept
            {
                // TODO
                _adlib->send(e);
            }
        }
    }
}