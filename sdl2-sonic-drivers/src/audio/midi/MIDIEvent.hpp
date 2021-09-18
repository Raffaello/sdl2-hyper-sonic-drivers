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
        protected:

            // MIDI event types (SysEx, Meta, Midi)
        };
    }
}
