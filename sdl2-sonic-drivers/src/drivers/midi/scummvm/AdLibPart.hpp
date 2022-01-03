#pragma once

#include <cstdint>

#include <drivers/midi/scummvm/MidiChannel.hpp>
#include <drivers/midi/scummvm/MidiDriver.hpp>
#include <drivers/midi/scummvm/AdLibInstrument.h>

namespace drivers
{
    namespace midi
    {
        namespace scummvm
        {
            struct AdLibVoice;
            class MidiDriver_ADLIB;

            class AdLibPart : public MidiChannel
            {
                friend class MidiDriver_ADLIB;

            protected:
                //	AdLibPart *_prev, *_next;
                AdLibVoice* _voice = nullptr;
                int16_t _pitchBend = 0;
                uint8_t _pitchBendFactor = 2;
                //int8_t _transposeEff;
                uint8_t _volEff = 0;
                int8_t _detuneEff = 0;
                uint8_t _modWheel = 0;
                bool _pedal = false;
                uint8_t _program = 0;
                uint8_t _priEff = 0;
                uint8_t _pan = 64;
                AdLibInstrument _partInstr;
                AdLibInstrument _partInstrSecondary;

                MidiDriver_ADLIB* _owner = nullptr;
                bool _allocated = false;
                uint8_t _channel = 0;

                void init(MidiDriver_ADLIB* owner, uint8_t channel);
                void allocate();

            public:
                AdLibPart();

                MidiDriver* device() override;
                uint8_t getNumber() override;
                void release() override;

                void send(uint32_t b) override;

                // Regular messages
                void noteOff(uint8_t note) override;
                void noteOn(uint8_t note, uint8_t velocity) override;
                void programChange(uint8_t program) override;
                void pitchBend(int16_t bend) override;

                // Control Change messages
                void controlChange(uint8_t control, uint8_t value) override;
                void modulationWheel(uint8_t value) override;
                void volume(uint8_t value) override;
                void panPosition(uint8_t value) override;
                void pitchBendFactor(uint8_t value) override;
                void detune(uint8_t value) override;
                void priority(uint8_t value) override;
                void sustain(bool value) override;
                void effectLevel(uint8_t value) override { return; } // Not supported
                void chorusLevel(uint8_t value) override { return; } // Not supported
                void allNotesOff() override;

                // SysEx messages
                void sysEx_customInstrument(uint32_t type, const uint8_t* instr) override;
            };
        }
    }
}
