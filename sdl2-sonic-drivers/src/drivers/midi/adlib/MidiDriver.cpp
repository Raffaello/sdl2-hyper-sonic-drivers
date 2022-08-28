#include <drivers/midi/adlib/MidiDriver.hpp>

namespace drivers
{
    namespace midi
    {
        namespace adlib
        {
            MidiDriver::MidiDriver(std::shared_ptr<hardware::opl::OPL> opl) : _opl(opl)
            {
                // TODO detect if OPL is a OPL2 ? 
                // TODO this need to refactor the opl namespaces etc..

            }

            void MidiDriver::send(const audio::midi::MIDIEvent& e) const noexcept
            {
                return;
            }
        }
    }
}