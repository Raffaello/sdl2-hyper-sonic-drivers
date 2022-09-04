#include <audio/MIDI.hpp>

namespace audio
{
    MIDI::MIDI(const midi::MIDI_FORMAT format, const uint16_t num_tracks, const uint16_t division)
        : format(format), numTracks(num_tracks), division(division)
    {
        _tracks.reserve(numTracks);
    }

    void MIDI::addTrack(const midi::MIDITrack& track)
    {
        if (_tracks.size() >= numTracks) {
            return;
        }

        // todo this if could be removed
        if (track.isLocked())
        {
            _tracks.push_back(track);
            return;
        }

        midi::MIDITrack t = track;

        t.lock();
        _tracks.push_back(t);
    }

    const midi::MIDITrack& MIDI::getTrack(const uint16_t track) const
    {
        return _tracks.at(track);
    }
}
