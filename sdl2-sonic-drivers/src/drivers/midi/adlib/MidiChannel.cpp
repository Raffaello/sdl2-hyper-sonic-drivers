#include <drivers/midi/adlib/MidiChannel.hpp>

namespace drivers
{
    namespace midi
    {
        namespace adlib
        {

            void MidiChannel::noteOff(uint8_t note) const
            {


            }

            void MidiChannel::noteOn(uint8_t note, uint8_t velocity) const
            {
                   
            }

            void MidiChannel::programChange(const uint8_t program, const files::dmx::OP2File::instrument_t& instrument)
            {
                //if (program > 127)
                //    return;

                _program = program;
                _instrument = _instrument;
            }
            const files::dmx::OP2File::instrument_t* MidiChannel::getInstrument() const noexcept
            {
                return &_instrument;
            }
        }
    }
}
