#pragma once

#include <cstdint>
#include <array>
#include <string>
#include <hardware/opl/OPL2instrument.h> // <- TODO: is this hardware or should be audio? 
                                         //          (hardware as it is strictly related to opl hardware)

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

            // TODO: promote to a class...
            typedef struct Op2Bank_t
            {
                std::array<Op2BankInstrument_t, OP2BANK_NUM_INSTRUMENTS> instruments;
                std::array<std::string, OP2BANK_NUM_INSTRUMENTS> names;
            } Op2Bank_t;
        }
    }
}
