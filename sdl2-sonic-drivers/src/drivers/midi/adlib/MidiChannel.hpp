#pragma once

#include <cstdint>
#include <audio/opl/banks/OP2Bank.hpp>

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
                MidiChannel() = default;
                ~MidiChannel() = default;
                

                uint8_t _instrument_number; // instrument # (rename to program)
                uint8_t _volume;            // volume
                uint8_t _pan;               // pan, 0=normal
                uint8_t _pitch;             // pitch wheel, 0=normal
                uint8_t _sustain;           // sustain pedal value
                uint8_t _modulation;        // modulation pot value

                audio::opl::banks::Op2BankInstrument_t _instrument;


                /// review below later on...

                //uint8_t getNumber() override;
                //void release() override;

                //void send(uint32_t b) override;

                // Regular messages
                void noteOff(const uint8_t note) const;
                void noteOn(const uint8_t note, const uint8_t velocity) const;
                void programChange(const uint8_t program, const audio::opl::banks::Op2BankInstrument_t& instrument);
                const audio::opl::banks::Op2BankInstrument_t* getInstrument() const noexcept;
                //void programChange(uint8_t program) override;
                //void pitchBend(int16_t bend) override;

                // Control Change messages
                //void controlChange(uint8_t control, uint8_t value) override;
                //void modulationWheel(uint8_t value) override;
                //void volume(uint8_t value) override;
                //void panPosition(uint8_t value) override;
                //void pitchBendFactor(uint8_t value) override;
                //void detune(uint8_t value) override;
                //void priority(uint8_t value) override;
                //void sustain(bool value) override;
                //void effectLevel(uint8_t value) override { return; } // Not supported
                //void chorusLevel(uint8_t value) override { return; } // Not supported
                //void allNotesOff() override;

            private:
                //uint8_t _program;
                //audio::opl::banks::Op2BankInstrument_t _instrument;
            };
        }
    }
}
