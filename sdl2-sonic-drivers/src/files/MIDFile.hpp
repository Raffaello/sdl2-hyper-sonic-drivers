#pragma once

#include <files/File.hpp>
#include <cstdint>

namespace files
{
    class MIDFile : public File
    {
    public:
        enum class eFormat
        {
            SINGLE_TRACK = 0,
            SIMULTANEOUS_TRACK = 1,
            MULTI_TRACK = 2
        };

        typedef struct MIDI_track_t
        {
            int32_t delta_time;
            //event_t 
        } MIDI_track_t;

        typedef struct MIDI_event_t
        {

        } MIDI_event_t;

        MIDFile(const std::string& filename);
        virtual ~MIDFile();
        /// <summary>
        /// Variable length quantity decoding algorithm
        /// </summary>
        /// <param name="encoded"></param>
        /// <returns></returns>
        static uint32_t decode_VLQ(const uint8_t buf[4]);

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

        midi_chunk_t read_chunk();
        void read_header();
        void check_format();


        
    };
}
