#pragma once

#include <cstdint>
#include <memory>
#include <audio/opl/banks/OP2Bank.hpp>

namespace drivers
{
    namespace midi
    {
        namespace adlib
        {
            class OplChannel
            {
            public:
                OplChannel() = delete;
                OplChannel(const bool isPercussion, const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank);
                ~OplChannel() = default;

                uint8_t volume = 0;            // volume
                uint8_t pan = 0;               // pan, 0=normal
                uint8_t pitch = 0;             // pitch wheel, 0=normal
                uint8_t sustain = 0;           // sustain pedal value
                uint8_t modulation = 0;        // modulation pot value // could be removed and replaced

                // TODO: this should return an OPL_BANK_INSTRUMENT_INTERFACE or something instead...
                const audio::opl::banks::Op2BankInstrument_t* setInstrument(const uint8_t note) noexcept;

                // Regular messages
                void programChange(const uint8_t program);

            private:
                const bool _isPercussion;
                uint8_t _program = 0;           // instrument number
                audio::opl::banks::Op2BankInstrument_t _instrument = { 0 };

                //const std::weak_ptr<audio::opl::banks::OP2Bank> _op2Bank;
                const audio::opl::banks::OP2Bank* _op2Bank;
            };
        }
    }
}
