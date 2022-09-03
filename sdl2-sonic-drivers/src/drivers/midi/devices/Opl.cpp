#include <drivers/midi/devices/Opl.hpp>
#include <cassert>

namespace drivers
{
    namespace midi
    {
        namespace devices
        {
            Opl::Opl(const std::shared_ptr<hardware::opl::OPL>& opl, const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank, const bool opl3_mode)
            {
                _opl = std::make_shared<drivers::midi::opl::OplDriver>(opl, op2Bank, opl3_mode);
            }

            void Opl::sendEvent(const audio::midi::MIDIEvent& e) const noexcept
            {
                _opl->send(e);
            }

            void Opl::sendMessage(const uint8_t msg[], const uint8_t size) const noexcept
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

            void Opl::sendSysEx(const audio::midi::MIDIEvent& e) const noexcept
            {
                // TODO
                //_adlib->send(e);
            }
        }
    }
}
