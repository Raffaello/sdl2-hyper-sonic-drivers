#pragma once

#include <audio/midi/MIDIEvent.hpp>
#include <vector>

namespace audio
{
    namespace midi
    {
        class MIDITrack
        {
        public:
            MIDITrack();
            MIDITrack(const std::vector<MIDIEvent> events);
            void addEvent(const MIDIEvent& e);
            // TODO move to protected later on..
            std::vector<MIDIEvent> events;
        protected:
            // TODO implement those below...
            /**
            * This tempo is in microseconds per minute, default 120BPM = 500000
            * MICROSECONDS_PER_MINUTE / _temp = Beats per minute.
            * MICROSECONDS_PER_MINUTE / BMP = _temp
            */
            uint32_t _tempo = 500000;

            uint8_t _numerator;
            /// <summary>
            /// negative power of 2 (2 = 1/4, 3 = 1/8, ...)
            /// </summary>
            uint8_t _denominator;
            uint8_t _midi_clocks_per_metronome_click;
            uint8_t _bb; // notated 32nd notes in what MIDI thinks of a quarter note???
            /// <summary>
            /// A positive value for the key specifies the number of sharps and a
            /// negative value specifies the number of flats.
            /// </summary>
            uint8_t _key;
            /// <summary>
            /// A value of 0 for the scale specifies a major key and a value of 1
            /// specifies a minor key.
            /// </summary>
            uint8_t _scale;

            uint32_t _curPos;
            uint32_t _ticks;
        };
    }
}
