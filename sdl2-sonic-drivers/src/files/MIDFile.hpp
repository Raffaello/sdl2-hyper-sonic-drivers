#pragma once

#include <files/File.hpp>
#include <cstdint>
#include <vector>

namespace files
{
    // TODO this file should just read the file, check is valid and return MIDI tracks.
    //      the content of midi tracks and midi events should be interpreted by the
    //      sequencer
    //      this file should return just tracks?
    //      with a sequence of raw events inside each track? (not interpreted)
    //      
    //      so decouple into a MIDI sequencer to stream the events into it?
    //      
    class MIDFile : public File
    {
    public:
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

        typedef struct MIDI_track_event_t
        {
            uint32_t delta_time;
            MIDI_EVENT_type_u type;
            std::vector<uint8_t> event_;
        } MIDI_event_t;

        typedef struct MIDI_track_t
        {
            std::vector<MIDI_track_event_t> events;
        } MIDI_track_t;


        MIDFile(const std::string& filename);
        virtual ~MIDFile();
        /// <summary>
        /// Variable length quantity decoding algorithm
        /// </summary>
        /// <param name="buf">the max 4 bytes array to decode</param>
        /// <param name="out_value">the resulting decoded value</param>
        /// <returns>byte reads</returns>
        static int decode_VLQ(const uint8_t buf[], uint32_t& out_value);
        int decode_VLQ(uint32_t& out_value);

    private:
        typedef struct midi_chunk_t
        {
            char id[4];
            uint32_t length;
        } midi_header_t;

        /**
        * Specifies the overall organization of the file. Only three values of
        * <format> are specified: 
        * 0 the file contains a single multi-channel track
        * 1 the file contains one or more simultaneous tracks
        *   (or MIDI outputs) of a sequence
        * 2 the file contains one or more sequentially independent
        *   single-track patterns
        * @see eFormat
        */
        int16_t _format = 0;
        /**
        * The number of track chunks in the file.
        * It will always be 1 for a format 0 file.
        */
        int16_t _nTracks = 0;
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
        int16_t _division = 0;



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

        midi_chunk_t read_chunk();
        void read_header();
        void check_format();

        std::vector<MIDI_track_t> _tracks;
    };
}
