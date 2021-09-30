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

        void reset() noexcept;
        
        int getMaxTicks() noexcept;
        //int getTotalTime() const noexcept;

        /**
        * Specifies the overall organization of the file. Only three values of
        * <format> are specified:
        * 0 the file contains a single multi-channel track
        * 1 the file contains one or more simultaneous tracks
        *   (or MIDI outputs) of a sequence
        * 2 the file contains one or more sequentially independent
        *   single-track patterns
        * @see midi::MIDI_FORMAT
        */
        const midi::MIDI_FORMAT format;
        /**
        * The number of track chunks in the file.
        * It will always be 1 for a format 0 file.
        */
        const uint16_t numTracks;
        /**
        * Specifies the meaning of the delta-times. It has two formats,
        * one for metrical time, and one for time-code-based time:
        * If bit 15 of <division> is a zero, the bits 14 thru 0 represent
        * the number of delta-time "ticks" which make up a quarter-note.
        * For instance, if <division> is 96, then a time interval of an
        * eighth-note between two events in the file would be 48.
        *
        * If bit 15 of <division> is a one, delta-times in a file
        * correspond to subdivisions of a second, in a way consistent
        * with SMPTE and MIDI time code. Bits 14 thru 8 contain one of
        * the four values -24, -25, -29, or -30, corresponding to the
        * four standard SMPTE and MIDI time code formats
        * (-29 corresponds to 30 drop frame),
        * and represents the number of frames per second
        */
        const uint16_t division;

    protected:
        std::vector<midi::MIDITrack> _tracks;
        int _maxTicks;
    };
}
