#pragma once

#include <audio/midi/types.hpp>
#include <cstdint>
#include <vector>
#include <memory>

namespace audio
{
    namespace midi
    {
        class MIDIEvent
        {
        public:
            // TODO build from a message array of 3 or 2 char.
            // TODO review the devices namespace to send only MIDIEvent
            explicit MIDIEvent() = default;
            ~MIDIEvent() = default;

            uint32_t delta_time = 0;
            MIDI_EVENT_type_u type = { 0 };
            midi_vector_t data;
            // removed abs_time as it is not a MIDIEvent.
            //uint32_t abs_time = 0; /// absolute ticks time derived from delta_time used for conversion.
        };
    }
}
