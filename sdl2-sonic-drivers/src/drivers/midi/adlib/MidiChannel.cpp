#include <drivers/midi/adlib/MidiChannel.hpp>
#include <spdlog/spdlog.h>

namespace drivers
{
    namespace midi
    {
        namespace adlib
        {
            MidiChannel::MidiChannel(const bool isPercussion, const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank) :
                _isPercussion(isPercussion), _op2Bank(op2Bank.get())
            {
            }

            void MidiChannel::noteOff(uint8_t note) const
            {


            }

            void MidiChannel::noteOn(uint8_t note, uint8_t velocity) const
            {
                   
            }

            void MidiChannel::programChange(const uint8_t program)
            {
                if (program > 127) {
                    spdlog::warn("Progam change value >= 127 -> {}", program);
                }

                // NOTE: if program is not changed shouldn't be required to do anything ...
                _program = program;
                _instrument = _op2Bank->getInstrument(program);
            }

            void modulationWheel(const uint8_t value) noexcept
            {
                // TODO
            }

            /*const audio::opl::banks::Op2BankInstrument_t* MidiChannel::getInstrument() const noexcept
            {
                return &_instrument;
            }*/

            const audio::opl::banks::Op2BankInstrument_t* MidiChannel::setInstrument(const uint8_t note) noexcept
            {
                if (_isPercussion)
                {
                    if (note < 35 || note > 81) {
                        spdlog::error("wrong percussion number {}", note);
                    }
                    _instrument = _op2Bank->getInstrument(note + (128 - 35));
                }

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
