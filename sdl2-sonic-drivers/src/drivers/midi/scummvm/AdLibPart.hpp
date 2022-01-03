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
                AdLibVoice* _voice;
                int16_t _pitchBend;
                uint8_t _pitchBendFactor;
                //int8_t _transposeEff;
                uint8_t _volEff;
                int8_t _detuneEff;
                uint8_t _modWheel;
                bool _pedal;
                uint8_t _program;
                uint8_t _priEff;
                uint8_t _pan;
                AdLibInstrument _partInstr;
                AdLibInstrument _partInstrSecondary;

            protected:
                MidiDriver_ADLIB* _owner;
                bool _allocated;
                uint8_t _channel;

                void init(MidiDriver_ADLIB* owner, uint8_t channel);
                void allocate();

            public:
                AdLibPart();

                MidiDriver* device();
                uint8_t getNumber();
                void release();

                void send(uint32_t b);

                // Regular messages
                void noteOff(uint8_t note);
                void noteOn(uint8_t note, uint8_t velocity);
                void programChange(uint8_t program);
                void pitchBend(int16_t bend);

                // Control Change messages
                void controlChange(uint8_t control, uint8_t value);
                void modulationWheel(uint8_t value);
                void volume(uint8_t value);
                void panPosition(uint8_t value);
                void pitchBendFactor(uint8_t value);
                void detune(uint8_t value);
                void priority(uint8_t value);
                void sustain(bool value);
                void effectLevel(uint8_t value) { return; } // Not supported
                void chorusLevel(uint8_t value) { return; } // Not supported
                void allNotesOff();

                // SysEx messages
                void sysEx_customInstrument(uint32_t type, const uint8_t* instr);
            };
        }
    }
}
