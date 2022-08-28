#include <memory>
#include <hardware/opl/OPL.hpp>
#include <audio/midi/MIDIEvent.hpp>

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

                void send(const audio::midi::MIDIEvent& e) const noexcept;

            private:
                std::shared_ptr<hardware::opl::OPL> _opl;
            };
        }
    }
}
