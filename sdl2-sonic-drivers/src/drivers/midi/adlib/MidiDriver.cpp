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
            }
        }
    }
}