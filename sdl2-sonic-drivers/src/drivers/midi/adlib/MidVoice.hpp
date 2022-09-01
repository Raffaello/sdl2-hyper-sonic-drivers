#pragma once

#include <cstdint>
#include <hardware/opl/OPL2instrument.h>

namespace drivers
{
    namespace midi
    {
        namespace adlib
        {
            // TODO rename to AdlibVoice and AbstractMidiVoice as interface
            class MidiVoice
            {
            public:
                MidiVoice() = default;
                ~MidiVoice() = default;

                uint8_t channel = 0;                        /* MIDI channel number */
                uint8_t note = 0;                           /* note number */
                uint8_t realnote = 0;                       /* adjusted note number */
                int8_t  finetune = 0;                       /* frequency fine-tune */
                int16_t pitch = 0;                          /* pitch-wheel value */
                uint8_t volume = 0;                         /* note volume */
                uint8_t realvolume = 0;                     /* adjusted note volume */

                // TODO: instead a MidiChannel should be Connected to a MidiVoice, i think...
                hardware::opl::OPL2instrument_t* instr = nullptr; /* current instrument */
                uint32_t time = 0;                                /* note start time */
                // Channel flags
                bool free = true;
                bool secondary = false;
                bool sustain = false;
                bool vibrato = false;
            };
        }
    }
}
