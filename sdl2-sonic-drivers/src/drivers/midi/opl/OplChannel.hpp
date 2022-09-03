#pragma once

#include <cstdint>
#include <memory>
#include <audio/opl/banks/OP2Bank.hpp>

namespace drivers
{
    namespace midi
    {
        namespace opl
        {
            /// <summary>
            /// More than one note can be played at once in one channel.
            /// It means more than one OplVoice can be associated to OplChannel.
            /// </summary>
            class OplChannel
            {
            public:
                OplChannel() = delete;
                OplChannel(const uint8_t channel_, const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank);
                ~OplChannel() = default;

                const uint8_t channel;         // MIDI channel, not used
                uint8_t volume = 0;            // volume
                uint8_t pan = 0;               // pan, 0=normal
                uint8_t pitch = 0;             // pitch wheel, 0=normal
                uint8_t sustain = 0;           // sustain pedal value
                uint8_t modulation = 0;        // modulation pot value

                // TODO: this should return an OPL_BANK_INSTRUMENT_INTERFACE or something instead...
                const audio::opl::banks::Op2BankInstrument_t* setInstrument(const uint8_t note) noexcept;

                // Regular messages
                void programChange(const uint8_t program);

            private:
                const bool _isPercussion;
                uint8_t _program = 0;           // instrument number
                // TODO: this should be a pointer to the bank instrument instead of copy it?
                audio::opl::banks::Op2BankInstrument_t _instrument = { 0 };
                const audio::opl::banks::OP2Bank* _op2Bank;
            };
        }
    }
}
