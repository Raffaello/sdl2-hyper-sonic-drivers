#pragma once

#include <files/File.hpp>
#include <string>
#include <array>
#include <cstdint>
#include <hardware/opl/OPL2instrument.h>
#include <audio/opl/banks/OP2Bank.h>
#include <memory>

namespace files
{
    namespace dmx
    {
       

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
           

            explicit OP2File(const std::string& filename);
            ~OP2File() override = default;

            audio::opl::banks::Op2BankInstrument_t getInstrument(const uint8_t i) const;
            std::string getInstrumentName(const uint8_t i) const;
            std::shared_ptr<audio::opl::banks::Op2Bank_t> getBank() const noexcept;
        private:
            std::shared_ptr<audio::opl::banks::Op2Bank_t> _bank;
            //std::array<std::string, audio::opl::banks::OP2BANK_NUM_INSTRUMENTS> _instrument_names;

            void _readInstrumentVoice(hardware::opl::OPL2instrument_t* buf);
            void _readInstrument(audio::opl::banks::Op2BankInstrument_t* buf);
            void _readInstruments();
            void _readInstrumentNames();
        };
    }
}
