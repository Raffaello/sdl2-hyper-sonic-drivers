#pragma once

#include <files/File.hpp>
#include <string>
#include <array>
#include <cstdint>
#include <drivers/midi/scummvm/AdLibInstrument.h>
#include <hardware/opl/OPL2instrument.h>

namespace files
{
    namespace dmx
    {
        constexpr int OP2FILE_NUM_INSTRUMENTS = 175;
        constexpr int OP2FILE_INSTRUMENT_NUM_VOICES = 2;

        /*
        * NOTE:
        * - The instruments array is mapped to OP2File instruments
        * - The MIDI program change, change instrument
        * - The MUS instrument array index is the same for the instrument
            specified in the OP2 Bank.
        */
        class OP2File : protected File
        {
        public:
            // TODO: make its own file instead OP2Bank_T
            typedef struct instrument_t
            {
                uint16_t flags;   /// Instrument flags: 0x01 - fixed pitch, 0x02 - delayed vibrato (unused),0x04 - Double-voice mode
                uint8_t fineTune; /// Second voice detune level
                uint8_t noteNum;  /// Percussion note number (between 0 and 128)
                std::array<hardware::opl::OPL2instrument_t, OP2FILE_INSTRUMENT_NUM_VOICES> voices;
            } instrument_t;

            explicit OP2File(const std::string& filename);
            ~OP2File() override = default;

            instrument_t getInstrument(const uint8_t i) const;
            std::string getInstrumentName(const uint8_t i) const;
        private:
            std::array<instrument_t, OP2FILE_NUM_INSTRUMENTS> _instruments;
            std::array<std::string, OP2FILE_NUM_INSTRUMENTS> _instrument_names;

            void _readInstrumentVoice(hardware::opl::OPL2instrument_t* buf);
            void _readInstrument(instrument_t* buf);
            void _readInstruments();
            void _readInstrumentNames();
        };
    }
}
