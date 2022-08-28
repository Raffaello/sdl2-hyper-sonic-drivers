#include <memory>
#include <hardware/opl/OPL.hpp>

namespace drivers
{
    namespace midi
    {
        namespace adlib
        {
            class MidiDriver
            {
            public:
                MidiDriver(std::shared_ptr<hardware::opl::OPL> opl);
                ~MidiDriver() = default;

            private:
                std::shared_ptr<hardware::opl::OPL> _opl;
            };
        }
    }
}
