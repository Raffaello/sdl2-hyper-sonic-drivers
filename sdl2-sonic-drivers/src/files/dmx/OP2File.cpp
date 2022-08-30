#include <files/dmx/OP2File.hpp>
#include <cstring>

namespace files
{
    namespace dmx
    {
        constexpr const char* OP2FILE_MAGIC_HEADER = "#OPL_II#";
        constexpr int OP2FILE_MAGIC_HEADER_SIZE = 8;
        constexpr int OP2FILE_INSTRUMENT_NAME_MAX_SIZE = 32;
        
        using audio::opl::banks::Op2BankInstrument_t;
        using audio::opl::banks::OP2BANK_INSTRUMENT_NUM_VOICES;
        using audio::opl::banks::OP2BANK_NUM_INSTRUMENTS;
        using audio::opl::banks::OP2Bank;

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

            _bank = std::make_shared<OP2Bank>(_instruments, _instrument_names);
        }

        /*Op2BankInstrument_t OP2File::getInstrument(const uint8_t i) const
        {
            return _instruments.at(i);
        }*/

        /*std::string OP2File::getInstrumentName(const uint8_t i) const
        {
            return _instrument_names.at(i);
        }*/

        const std::shared_ptr<audio::opl::banks::OP2Bank> OP2File::getBank() const noexcept
        {
            return _bank;
        }

        void OP2File::_readInstrumentVoice(hardware::opl::OPL2instrument_t* buf)
        {
            read(buf, sizeof(hardware::opl::OPL2instrument_t));
        }

        void OP2File::_readInstrument(Op2BankInstrument_t* buf)
        {
            buf->flags = readLE16();
            buf->fineTune = readU8();
            buf->noteNum = readU8();
            for (int i = 0; i < OP2BANK_INSTRUMENT_NUM_VOICES; i++) {
                _readInstrumentVoice(&(buf->voices[i]));
            }
        }

        void OP2File::_readInstruments()
        {
            for (int i = 0; i < OP2BANK_NUM_INSTRUMENTS; i++) {
                _readInstrument(&_instruments[i]);
            }
        }

        void OP2File::_readInstrumentNames()
        {
            for (int i = 0; i < OP2BANK_NUM_INSTRUMENTS; i++) {
                _instrument_names[i].reserve(OP2FILE_INSTRUMENT_NAME_MAX_SIZE);
                _instrument_names[i] = _readStringFromFile();
                _instrument_names[i].shrink_to_fit();
                seek(OP2FILE_INSTRUMENT_NAME_MAX_SIZE - 1 - _instrument_names[i].size(), std::ios::cur);
            }
        }
    }
}
