#include <audio/midi/MIDITrack.hpp>

namespace audio
{
    namespace midi
    {
        MIDITrack::MIDITrack(const std::vector<MIDIEvent> events) : MIDITrack()
        {
            _events = events;
        }

        void MIDITrack::addEvent(const MIDIEvent& e)
        {
            _events.push_back(e);
        }
    }
}
