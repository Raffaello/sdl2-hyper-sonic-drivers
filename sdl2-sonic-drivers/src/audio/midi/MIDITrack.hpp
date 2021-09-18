#pragma once

#include <audio/midi/MIDIEvent.hpp>
#include <vector>

namespace audio
{
    namespace midi
    {
        class MIDITrack
        {
        public:
            MIDITrack();
            MIDITrack(const std::vector<MIDIEvent> events);
            void addEvent(const MIDIEvent& e);
        protected:
            std::vector<MIDIEvent> _events;
            uint32_t _curPos;
            uint32_t _ticks;
        };
    }
}
