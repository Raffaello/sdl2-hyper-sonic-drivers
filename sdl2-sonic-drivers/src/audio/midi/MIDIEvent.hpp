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
            MIDIEvent();

            uint32_t delta_time;
            MIDI_EVENT_type_u type;
            std::vector<uint8_t> data;
            uint32_t abs_time; // absolute ticks time, derived from delta_time, used for conversion.
        protected:

            // MIDI event types (SysEx, Meta, Midi)
        };
    }
}
