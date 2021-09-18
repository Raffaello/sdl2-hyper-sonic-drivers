#pragma once

#include <vector>
#include <cstdint>

namespace audio
{
    namespace midi
    {
        enum class MIDI_FORMAT
        {
            SINGLE_TRACK = 0,
            SIMULTANEOUS_TRACK = 1,
            MULTI_TRACK = 2
        };

        typedef union MIDI_EVENT_type_u
        {
            uint8_t val;
            struct {
                uint8_t low : 4;
                uint8_t high : 4;
            };
        } MIDI_EVENT_type_u;

        enum class MIDI_META_EVENT
        {
            SEQUENCE_NUMBER = 0x00,
            TEXT = 0x01,
            COPYRIGHT = 0x02,
            SEQUENCE_NAME = 0x03,
            INSTRUMENT_NAME = 0x04,
            LYRICS = 0x05,
            MARKER = 0x06,
            CUE_POINT = 0x07,
            CHANNEL_PREFIX = 0x20,
            MIDI_PORT = 0x21,
            END_OF_TRACK = 0x2F,
            SET_TEMPO = 0x51, // in microseconds per MIDI quarter note
            SMPTE_OFFSET = 0x54,
            TIME_SIGNATURE = 0x58,
            KEY_SIGNATURE = 0x59,
            SEQUENCER_SPECIFIC = 0x7F
        };

        // TODO: to remove and convert as classes?
        typedef struct MIDI_track_event_t
        {
            uint32_t delta_time;
            MIDI_EVENT_type_u type;
            std::vector<uint8_t> data; // MIDI events 1 or 2 bytes, sysEx/MetaEvent varies.
        } MIDI_event_t;

        // This could be a class for processing format 2
        // with its own tempo, key, time, etc... 
        typedef struct MIDI_track_t
        {
            std::vector<MIDI_track_event_t> events;
            uint32_t cur_pos = 0; // events current position index
            uint32_t ticks = 0;   // track ticks
        } MIDI_track_t;
    }
}
