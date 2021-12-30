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

            uint32_t delta_time = 0;
            MIDI_EVENT_type_u type;
            std::vector<uint8_t> data;
            uint32_t abs_time = 0; /// absolute ticks time derived from delta_time used for conversion.
        };
    }
}
