#include <audio/midi/MIDITrack.hpp>

namespace audio
{
    namespace midi
    {
        MIDITrack::MIDITrack() /* : _curPos(0), _ticks(0) */
        {
        }

        MIDITrack::MIDITrack(const std::vector<MIDIEvent> events) : MIDITrack()
        {
            this->events = events;
        }

        void MIDITrack::addEvent(const MIDIEvent& e)
        {
            events.push_back(e);
        }
    }
}
