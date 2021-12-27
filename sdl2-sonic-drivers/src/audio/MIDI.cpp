#include <audio/MIDI.hpp>

namespace audio
{
    MIDI::MIDI(const midi::MIDI_FORMAT format, const uint16_t num_tracks, const uint16_t division)
        : format(format), numTracks(num_tracks), division(division), _maxTicks(0)
    {
        _tracks.reserve(numTracks);
    }

    void MIDI::addTrack(const midi::MIDITrack& track)
    {
        //TODO: it is possible to add tracks
        //BODY: that it shouldn't from public interface
        if (_tracks.size() >= numTracks) {
            //return;
        }

        _tracks.emplace_back(track);
    }

    const midi::MIDITrack& MIDI::getTrack(const uint16_t track) const
    {
        return _tracks.at(track);
    }

    void MIDI::reset() noexcept
    {
        for (auto& track : _tracks)
            track.reset();
    }

    int MIDI::getMaxTicks() noexcept
    {
        // TODO: format 2 won't be right.
        // BODY: to move into MIDITrack class
        if (_maxTicks == 0)
        {
            for (auto& t : _tracks)
            {
                int sum = 0;
                for (auto& e : t.events) {
                    sum += e.delta_time;
                }

                if (sum > _maxTicks) {
                    _maxTicks = sum;
                }
            }

            // max / (_tempo / _division) (with _divsion bit 15 = 0)
        }

        return _maxTicks;
    }
}
