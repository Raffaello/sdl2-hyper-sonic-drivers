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
            MIDIEvent(const uint32_t delta_time, const MIDI_EVENT_type_u type, const std::vector<uint8_t>& data);
            MIDIEvent(const uint32_t delta_time, const MIDI_EVENT_type_u type, const std::shared_ptr<uint8_t[]> data, const int dataSize);

            void setEvent(const uint32_t delta_time, const MIDI_EVENT_type_u type, const std::vector<uint8_t>& data);

            uint32_t delta_time;
            MIDI_EVENT_type_u type;
        protected:
            std::vector<uint8_t> _data;

            // MIDI event types (SysEx, Meta, Midi)

        };
    }
}
