#pragma once

#include <drivers/midi/scummvm/AdLibPart.hpp>
#include <drivers/midi/scummvm/AdLibInstrument.h>
#include <cstdint>


namespace drivers
{
    namespace midi
    {
        namespace scummvm
        {
            // FYI (Jamieson630)
            // It is assumed that any invocation to AdLibPercussionChannel
            // will be done through the MidiChannel base class as opposed to the
            // AdLibPart base class. If this were NOT the case, all the functions
            // listed below would need to be virtual in AdLibPart as well as MidiChannel.
            class AdLibPercussionChannel : public AdLibPart
            {
                friend class MidiDriver_ADLIB;

            protected:
                void init(MidiDriver_ADLIB* owner, uint8_t channel);

            public:
                ~AdLibPercussionChannel();

                void noteOff(uint8_t note);
                void noteOn(uint8_t note, uint8_t velocity);
                void programChange(uint8_t program) { }

                // Control Change messages
                void modulationWheel(uint8_t value) { }
                void pitchBendFactor(uint8_t value) { }
                void detune(uint8_t value) { }
                void priority(uint8_t value) { }
                void sustain(bool value) { }

                // SysEx messages
                void sysEx_customInstrument(uint32_t type, const uint8_t* instr);

            private:
                uint8_t _notes[256];
                AdLibInstrument* _customInstruments[256];
            };
        }
    }
}
