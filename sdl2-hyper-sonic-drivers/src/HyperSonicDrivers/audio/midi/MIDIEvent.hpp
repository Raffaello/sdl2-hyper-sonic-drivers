#pragma once

#include <HyperSonicDrivers/audio/midi/types.hpp>
#include <cstdint>
#include <vector>
#include <memory>

namespace HyperSonicDrivers::audio::midi
{
    class MIDIEvent
    {
    public:
        // TODO build from a message array of 3 or 2 char.
        explicit MIDIEvent() = default;
        ~MIDIEvent() = default;

        uint32_t delta_time = 0;
        MIDI_EVENT_type_u type = { 0 };
        midi_vector_t data;

        inline uint32_t toUint32() const noexcept
        {
            if (data.empty())
                return 0;

            uint32_t res = data[0];
            if (data.size() == 2)
                res = (res << 8) | (data[1]);

            return (res << 8) | type.val;
        }
        // removed abs_time as it is not a MIDIEvent.
        //uint32_t abs_time = 0; /// absolute ticks time derived from delta_time used for conversion.
    };
}
