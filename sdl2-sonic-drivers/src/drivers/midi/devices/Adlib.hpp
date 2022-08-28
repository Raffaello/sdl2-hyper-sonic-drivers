#include <drivers/midi/Device.hpp>
#include <memory>
#include <hardware/opl/OPL.hpp>
#include <drivers/midi/adlib/MidiDriver.hpp>

namespace drivers
{
    namespace midi
    {
        namespace devices
        {
            class Adlib : public Device
            {
            public:
                Adlib(std::shared_ptr<hardware::opl::OPL> opl);
                ~Adlib() = default;

            private:
                std::shared_ptr<drivers::midi::adlib::MidiDriver> _adlib;
            };
        }
    }
}
