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

        _tracks.emplace_back(track);
    }

    const midi::MIDITrack& MIDI::getTrack(const uint16_t track) const
    {
        return _tracks.at(track);
    }
}
