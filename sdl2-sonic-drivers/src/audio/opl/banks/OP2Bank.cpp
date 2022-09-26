#include <audio/opl/banks/OP2Bank.hpp>
#include <spdlog/spdlog.h>

namespace audio::opl::banks
{
    OP2Bank::OP2Bank(
        const std::array<Op2BankInstrument_t, OP2BANK_NUM_INSTRUMENTS>& instruments,
        const std::array<std::string, OP2BANK_NUM_INSTRUMENTS>& names
    ) : _instruments(instruments), _names(names)
    {
    }

    uint8_t OP2Bank::getPercussionIndex(const uint8_t note)
    {
        if (note < 35 || note > 81) {
            spdlog::error("wrong percussion number {}", note);
        }

        return note + (128 - 35);
    }

    bool OP2Bank::supportOpl3(const Op2BankInstrument_t* instr)
    {
        return (instr->flags & OP2BANK_INSTRUMENT_FLAG_DOUBLE_VOICE) != 0;
    }

    bool OP2Bank::isPercussion(const Op2BankInstrument_t* instr)
    {
        return (instr->flags & OP2BANK_INSTRUMENT_FLAG_FIXED_PITCH) != 0;
    }

    Op2BankInstrument_t OP2Bank::getInstrument(const uint8_t i) const
    {
        return _instruments.at(i);
    }

    std::string OP2Bank::getInstrumentName(const uint8_t i) const
    {
        return _names.at(i);
    }

    Op2BankInstrument_t OP2Bank::getPercussionInstrument(const uint8_t note) const
    {
        return getInstrument(getPercussionIndex(note));
    }

    const Op2BankInstrument_t* OP2Bank::getPercussionInstrumentPtr(const uint8_t note) const
    {
        return &(_instruments.at(getPercussionIndex(note)));
    }

    const Op2BankInstrument_t* OP2Bank::getInstrumentPtr(const uint8_t i) const
    {
        return &_instruments.at(i);
    }
}
