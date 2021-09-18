#include "MIDIEvent.hpp"
#include <audio/midi/MIDIEvent.hpp>

namespace audio
{
    namespace midi
    {
        MIDIEvent::MIDIEvent() : delta_time(0), type(0)
        {
        }

        MIDIEvent::MIDIEvent(const uint32_t delta_time, const MIDI_EVENT_type_u type, const std::vector<uint8_t>& data)
            : delta_time(delta_time), type(type), _data(data)
        {
        }
        MIDIEvent::MIDIEvent(const uint32_t delta_time, const MIDI_EVENT_type_u type, const std::shared_ptr<uint8_t[]> data, const int dataSize)
            : delta_time(delta_time), type(type)
        {
            _data = std::vector<uint8_t>(data.get(), dataSize);
        }
        void MIDIEvent::setEvent(const uint32_t delta_time, const MIDI_EVENT_type_u type, const std::vector<uint8_t>& data)
        {
        }
    }
}
