#include <files/dmx/OP2File.hpp>
#include <cstring>

namespace files
{
    namespace dmx
    {
        constexpr const char* OP2FILE_MAGIC_HEADER = "#OPL_II#";
        constexpr int OP2FILE_MAGIC_HEADER_SIZE = 8;
        constexpr int OP2FILE_INSTRUMENT_NAME_MAX_SIZE = 32;
        constexpr int OP2FILE_INSTRUMENT_NUM_VOICES = 2;

        OP2File::OP2File(const std::string& filename) : File(filename)
        {
            // header
            char buf[OP2FILE_MAGIC_HEADER_SIZE + 1];
            read(buf, OP2FILE_MAGIC_HEADER_SIZE);
            buf[OP2FILE_MAGIC_HEADER_SIZE] = 0;
            _assertValid(strncmp(buf, OP2FILE_MAGIC_HEADER, OP2FILE_MAGIC_HEADER_SIZE) == 0);

            // instruments
            _readInstruments();

            // instrument names
            _readInstrumentNames();
        }

        OP2File::instrument_t OP2File::getInstrument(const uint8_t i) const
        {
            return _instruments.at(i);
        }

        std::string OP2File::getInstrumentName(const uint8_t i) const
        {
            return _instrument_names.at(i);
        }

        drivers::midi::scummvm::AdLibInstrument OP2File::getInstrumentToAdlib(const uint8_t i) const
        {
            // TODO this is not working with the current ScummVM:MidiDriver_Adlib (need a general adlib midi driver)
            auto instr = _instruments.at(i);
            drivers::midi::scummvm::AdLibInstrument adlib;

            // e.g. of a current instrument vs op2 file (both overdrive gtr):
            // { 0xC2, 0x2E, 0x4F, 0x77, 0x00, 0xC4, 0x08, 0x0E, 0x98, 0x59, 0x0A, 0, { 0, 0, 0, 0, 0, 0, 0, 0 }, 0, { 0, 0, 0, 0, 0, 0, 0, 0 }, 0x00 },
            // { 0x10, 0x00, 0xF1, 0xFF, 0x00, 0x51, 0x00, 0xF0, 0xFF, 0x01, 0x0C, 0, { 0, 0, 0, 0, 0, 0, 0, 0 }, 0, { 0, 0, 0, 0, 0, 0, 0, 0 }, 0x00 },
               
            adlib.modCharacteristic = instr.voices[0].iModChar;
            adlib.modScalingOutputLevel = instr.voices[0].iModScale;
            adlib.modAttackDecay = instr.voices[0].iModAttack;
            adlib.modSustainRelease = instr.voices[0].iModSustain;
            adlib.modWaveformSelect = instr.voices[0].iModWaveSel;
            adlib.carCharacteristic = instr.voices[0].iCarChar;
            adlib.carScalingOutputLevel = instr.voices[0].iCarScale;
            adlib.carAttackDecay = instr.voices[0].iCarAttack;
            adlib.carSustainRelease = instr.voices[0].iCarSustain;
            adlib.carWaveformSelect = instr.voices[0].iCarWaveSel;
            adlib.feedback = instr.voices[0].iFeedback;
            adlib.flagsA = 0; //instr.voices[0].reserved;
            adlib.extraA = { 0, 0, 0, 0, 0, 0, 0, 0 };
            adlib.flagsB = 0;
            adlib.extraB = { 0, 0, 0, 0, 0, 0, 0, 0 };
            adlib.duration = 0; // instr.voices[0].noteOffset;
            
            return adlib;
        }

        void OP2File::_readInstrumentVoice(instrument_voice_t* buf)
        {
            read(buf, sizeof(instrument_voice_t));
        }

        void OP2File::_readInstrument(instrument_t* buf)
        {
            buf->flags = readLE16();
            buf->fineTune = readU8();
            buf->noteNum = readU8();
            for (int i = 0; i < OP2FILE_INSTRUMENT_NUM_VOICES; i++) {
                _readInstrumentVoice(&(buf->voices[i]));
            }
        }

        void OP2File::_readInstruments()
        {
            for (int i = 0; i < OP2FILE_NUM_INSTRUMENTS; i++) {
                _readInstrument(&_instruments[i]);
            }
        }

        void OP2File::_readInstrumentNames()
        {
            for (int i = 0; i < OP2FILE_NUM_INSTRUMENTS; i++) {
                _instrument_names[i].reserve(OP2FILE_INSTRUMENT_NAME_MAX_SIZE);
                _instrument_names[i] = _readStringFromFile();
                _instrument_names[i].shrink_to_fit();
                seek(OP2FILE_INSTRUMENT_NAME_MAX_SIZE - 1 - _instrument_names[i].size(), std::ios::cur);
            }
        }
    }
}
