#include <HyperSonicDrivers/audio/MIDI.hpp>

namespace HyperSonicDrivers::audio
{
MIDI::MIDI(const midi::MIDI_FORMAT format, const uint16_t num_tracks, const uint16_t division)
    : format(format), numTracks(num_tracks), division(division)
{
    m_tracks.reserve(numTracks);
}

void MIDI::addTrack(const midi::MIDITrack& track)
{
    if (m_tracks.size() >= numTracks)
    {
        return;
    }

    // todo this if could be removed
    if (track.isLocked())
    {
        m_tracks.push_back(track);
        return;
    }

    midi::MIDITrack t = track;

    t.lock();
    m_tracks.push_back(t);
}

const midi::MIDITrack& MIDI::getTrack(const uint16_t track) const
{
    return m_tracks.at(track);
}
}    // namespace HyperSonicDrivers::audio
