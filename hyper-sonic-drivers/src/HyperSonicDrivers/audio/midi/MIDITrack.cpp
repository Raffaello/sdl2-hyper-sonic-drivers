#include <HyperSonicDrivers/audio/midi/MIDITrack.hpp>

namespace HyperSonicDrivers::audio::midi
{
MIDITrack::MIDITrack(const std::vector<MIDIEvent>& events) : MIDITrack()
{
    m_events = events;
}

void MIDITrack::addEvent(const MIDIEvent& e)
{
    if (m_lock)
        return;

    MIDIEvent ev = e;

    ev.data.shrink_to_fit();
    m_events.emplace_back(ev);
}

void MIDITrack::lock() noexcept
{
    m_events.shrink_to_fit();
    m_lock = true;
}
}    // namespace HyperSonicDrivers::audio::midi
