#include <drivers/midi/adlib/MidiChannel.hpp>
#include <spdlog/spdlog.h>

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

            void MidiChannel::programChange(const uint8_t program, const audio::opl::banks::Op2BankInstrument_t& instrument)
            {
                if (program > 127) {
                    spdlog::warn("Progam change value >= 127 -> {}", program);
                }


                _program = program;
                _instrument = instrument;
            }

            void modulationWheel(const uint8_t value) noexcept
            {
                // TODO
            }

            const audio::opl::banks::Op2BankInstrument_t* MidiChannel::getInstrument() const noexcept
            {
                return &_instrument;
            }

            void MidiChannel::pitchBend(const int16_t bend) noexcept
            {
                // TODO requires voices
               //_pitch = static_cast<int8_t>(bend);
            }
        }
    }
}
