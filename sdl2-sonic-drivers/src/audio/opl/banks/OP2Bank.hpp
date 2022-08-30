#pragma once

#include <cstdint>
#include <array>
#include <string>
#include <hardware/opl/OPL2instrument.h> 

namespace audio
{
    namespace opl
    {
        namespace banks
        {
            /* This is the content of a OP2File */

            constexpr int OP2BANK_INSTRUMENT_NUM_VOICES = 2;
            constexpr int OP2BANK_NUM_INSTRUMENTS = 175;

            typedef struct Op2BankInstrument_t
            {
                uint16_t flags;   /// Instrument flags: 0x01 - fixed pitch, 0x02 - delayed vibrato (unused),0x04 - Double-voice mode
                uint8_t fineTune; /// Second voice detune level
                uint8_t noteNum;  /// Percussion note number (between 0 and 128)
                std::array<hardware::opl::OPL2instrument_t, OP2BANK_INSTRUMENT_NUM_VOICES> voices;
            } Op2BankInstrument_t;

            class OP2Bank
            {
            public:
                OP2Bank(
                    const std::array<Op2BankInstrument_t, OP2BANK_NUM_INSTRUMENTS>& instruments,
                    const std::array<std::string, OP2BANK_NUM_INSTRUMENTS>& names
                );
                ~OP2Bank() = default;

                // TODO replace with a shared pointer!!!
                
                /*inline Op2BankInstrument_t* getInstrumentPtr(const uint8_t i) const {
                    return &_instruments.at(i);
                }*/
                Op2BankInstrument_t getInstrument(const uint8_t i) const;

                std::string getInstrumentName(const uint8_t i) const;

            private:
                const std::array<Op2BankInstrument_t, OP2BANK_NUM_INSTRUMENTS> _instruments;
                const std::array<std::string, OP2BANK_NUM_INSTRUMENTS> _names;
            };
        }
    }
}
