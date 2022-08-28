#include <drivers/midi/devices/Adlib.hpp>

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
        }
    }
}