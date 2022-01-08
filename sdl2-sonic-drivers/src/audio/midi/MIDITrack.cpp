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
            if (_lock)
                return;

            MIDIEvent ev = e;

            ev.data.shrink_to_fit();
            _events.push_back(ev);
        }

        void MIDITrack::lock() noexcept
        {
            _events.shrink_to_fit();
            _lock = true;
        }
    }
}
