#pragma once

#include <cstdint>
#include <memory>
#include <audio/opl/banks/OP2Bank.hpp>
#include <drivers/midi/adlib/MidVoice.hpp>

namespace drivers
{
    namespace midi
    {
        namespace adlib
        {
            // TODO rename OPLChannel, in this case is OPL2Channel (AdLib Channel)
            class MidiChannel
            {
            public:
                MidiChannel() = delete;
                MidiChannel(const bool isPercussion, const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank);
                ~MidiChannel() = default;


                uint8_t _volume = 0;            // volume
                uint8_t _pan = 0;               // pan, 0=normal
                uint8_t _pitch = 0;             // pitch wheel, 0=normal
                uint8_t _sustain = 0;           // sustain pedal value
                uint8_t _modulation = 0;        // modulation pot value


                const audio::opl::banks::Op2BankInstrument_t* setInstrument(const uint8_t note) noexcept;
                inline bool isVoiceFree() const noexcept;
                MidiVoice* _voice = nullptr; // the OPL Channel associated to this Midi Channel


                /// review below later on...
                //uint8_t getNumber() override;
                //void release() override;
                //void send(uint32_t b) override;

                // Regular messages
                void noteOff(const uint8_t note) const;
                void noteOn(const uint8_t note, const uint8_t velocity) const;
                void programChange(const uint8_t program);
                void pitchBend(const int16_t bend) noexcept;

                
                // Control Change messages
                //void controlChange(uint8_t control, uint8_t value) override;
                void modulationWheel(const uint8_t value) noexcept;
                void volume(const uint8_t value) noexcept;
                //void panPosition(uint8_t value) override;
                //void pitchBendFactor(uint8_t value) override;
                //void detune(uint8_t value) override;
                //void priority(uint8_t value) override;
                //void sustain(bool value) override;
                //void effectLevel(uint8_t value) override { return; } // Not supported
                //void chorusLevel(uint8_t value) override { return; } // Not supported
                //void allNotesOff() override;

            private:
                const bool _isPercussion;
                uint8_t _program = 0;           // instrument number
                audio::opl::banks::Op2BankInstrument_t _instrument = { 0 };
                //uint8_t _volume = 0;            // volume



                //const std::shared_ptr<audio::opl::banks::OP2Bank> _op2Bank;
                //const std::weak_ptr<audio::opl::banks::OP2Bank> _op2Bank;
                const audio::opl::banks::OP2Bank* _op2Bank;
                //MidiVoice* _voice = nullptr; // the OPL Channel associated to this Midi Channel
            };
        }
    }
}
