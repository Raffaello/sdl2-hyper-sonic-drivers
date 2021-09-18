#pragma once

#include <cstdint>
#include <vector>
#include <audio/midi/types.hpp>
#include <audio/midi/MIDITrack.hpp>

namespace audio
{
    class MIDI
    {
    public:
        MIDI(const midi::MIDI_FORMAT format, const uint16_t num_tracks, const uint16_t division);

        void addTrack(const midi::MIDITrack& track);
        const midi::MIDITrack& getTrack(const uint16_t track) const;
        
        int getMaxTicks() noexcept;
        //int getTotalTime() const noexcept;

        const midi::MIDI_FORMAT format;
        const uint16_t numTracks;
        const uint16_t division;

    protected:
        std::vector<midi::MIDITrack> _tracks;
        int _maxTicks;
    };
}
