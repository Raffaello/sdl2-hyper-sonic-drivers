#include <drivers/midi/devices/OPL.hpp>

namespace drivers
{
    namespace midi
    {
        namespace devices
        {
            OPL::OPL(std::shared_ptr<hardware::opl::OPL> opl) : _opl(opl)
            {
            }

            inline void OPL::sendEvent(const audio::midi::MIDIEvent& e) const noexcept
            {
            }

            inline void OPL::sendMessage(const uint8_t msg[], const uint8_t size) const noexcept
            {
            }
        }
    }
}