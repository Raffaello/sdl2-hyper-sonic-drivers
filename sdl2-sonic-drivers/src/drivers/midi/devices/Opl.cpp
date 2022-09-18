#include <drivers/midi/devices/Opl.hpp>
#include <cassert>

namespace drivers
{
    namespace midi
    {
        namespace devices
        {
            Opl::Opl(const std::shared_ptr<hardware::opl::OPL>& opl, const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank, const bool opl3_mode)
                : Device()/*, OplDriver(opl, op2Bank, opl3_mode)*/
            {
                // TODO: if opl is not opl3 opl3_mode should be force to be false as the "hardware"
                //       doesn't support opl3 mode.
                //       This need to take the OPL type from OPL interface.
                _oplDriver = std::make_shared<drivers::midi::opl::OplDriver>(opl, op2Bank, opl3_mode);
            }

            void Opl::sendEvent(const audio::midi::MIDIEvent& e) const noexcept
            {
                //this->send(e);
                _oplDriver->send(e);
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

            void Opl::pause() const noexcept
            {
                _oplDriver->pause();

            }

            void Opl::resume() const noexcept
            {
                _oplDriver->resume();
            }

        }
    }
}
