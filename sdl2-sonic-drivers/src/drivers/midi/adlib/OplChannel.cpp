#include <drivers/midi/adlib/OplChannel.hpp>
#include <spdlog/spdlog.h>

namespace drivers
{
    namespace midi
    {
        namespace adlib
        {
            OplChannel::OplChannel(const bool isPercussion, const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank) :
                _isPercussion(isPercussion), _op2Bank(op2Bank.get())
            {
            }

            const audio::opl::banks::Op2BankInstrument_t* OplChannel::setInstrument(const uint8_t note) noexcept
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

            void OplChannel::programChange(const uint8_t program)
            {
                if (program > 127) {
                    spdlog::warn("Progam change value >= 127 -> {}", program);
                }

                // NOTE: if program is not changed shouldn't be required to do anything ...
                _program = program;
                _instrument = _op2Bank->getInstrument(program);
            }
        }
    }
}
