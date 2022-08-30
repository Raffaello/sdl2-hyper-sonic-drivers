#include <drivers/midi/devices/Adlib.hpp>
#include <cassert>
#include <array>

namespace drivers
{
    namespace midi
    {
        namespace devices
        {
            Adlib::Adlib(const std::shared_ptr<hardware::opl::OPL>& opl, const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank)
            {
                _adlib = std::make_shared<drivers::midi::adlib::MidiDriver>(opl,op2Bank);
            }

            void Adlib::sendEvent(const audio::midi::MIDIEvent& e) const noexcept
            {
                _adlib->send(e);
            }

            void Adlib::sendMessage(const uint8_t msg[], const uint8_t size) const noexcept
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

            void Adlib::sendSysEx(const audio::midi::MIDIEvent& e) const noexcept
            {
                // TODO
                //_adlib->send(e);
            }
        }
    }
}
