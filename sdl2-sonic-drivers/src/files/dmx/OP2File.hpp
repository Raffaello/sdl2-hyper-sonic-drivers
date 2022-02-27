#pragma once

#include <files/File.hpp>
#include <string>
#include <array>
#include <cstdint>

namespace files
{
    namespace dmx
    {
        constexpr int OP2FILE_NUM_INSTRUMENTS = 175;

        class OP2File : protected File
        {
        public:
            typedef struct instrument_voice_t
            {
                uint8_t iModChar;    /// Modulator characteristic (Mult, KSR, EG, VIB and AM flags)
                uint8_t iModAttack;  /// Modulator attack/decay level
                uint8_t iModSustain; /// Modulator sustain/release level
                uint8_t iModWaveSel; /// Modulator wave select
                uint8_t iModScale;   /// Modulator key scaling (first two bits)
                uint8_t iModLevel;   /// Modulator output level (last six bits)
                uint8_t iFeedback;   /// Feedback/connection
                uint8_t iCarChar;    /// Carrier characteristic (Mult, KSR, EG, VIB and AM flags)
                uint8_t iCarAttack;  /// Carrier attack / decay level
                uint8_t iCarSustain; /// Carrier sustain/release level
                uint8_t iCarWaveSel; /// Carrier wave select
                uint8_t iCarScale;   /// Carrier key scaling(first two bits)
                uint8_t iCarLevel;   /// Carrier output level (last six bits)
                uint8_t reserved;    /// Unused byte
                int16_t noteOffset;  /// MIDI note offset for a first voice
            } instrument_voice_t;

            typedef struct instrument_t
            {
                uint16_t flags;   /// Instrument flags: 0x01 - fixed pitch, 0x02 - delayed vibrato (unused),0x04 - Double-voice mode
                uint8_t fineTune; /// Second voice detune level
                uint8_t noteNum;  /// Percussion note number (between 0 and 128)
                std::array<instrument_voice_t, 2> voices;
            } instrument_t;

            explicit OP2File(const std::string& filename);
            ~OP2File() override = default;

            instrument_t getInstrument(const int i) const noexcept;
            std::string getInstrumentName(const int i) const noexcept;

        private:
            std::array<instrument_t, OP2FILE_NUM_INSTRUMENTS> _instruments;
            std::array<std::string, OP2FILE_NUM_INSTRUMENTS> _instrument_names;

            void _readInstrumentVoice(instrument_voice_t* buf);
            void _readInstrument(instrument_t* buf);
            void _readInstruments();
            void _readInstrumentNames();
        };
    }
}
