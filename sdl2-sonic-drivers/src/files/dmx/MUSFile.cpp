#include <files/dmx/MUSFile.hpp>
#include <utils/endianness.hpp>
#include <audio/midi/types.hpp>
#include <cstring>

namespace files
{
    namespace dmx
    {
        constexpr const char ID_MAGIC[4] = { 'M','U','S','\x1A' };
        constexpr int MAX_SIZE = 1024 * 64;
        constexpr int MAX_CHANNELS = 9;

        constexpr int MUS_EVENT_TYPE_RELEASE_NOTE = 0;
        constexpr int MUS_EVENT_TYPE_PLAY_NOTE = 1;
        constexpr int MUS_EVENT_TYPE_PITCH_BEND = 2;
        constexpr int MUS_EVENT_TYPE_SYS_EVENT = 3;
        constexpr int MUS_EVENT_TYPE_CONTROLLER = 4;
        constexpr int MUS_EVENT_TYPE_END_OF_MEASURE = 5;
        constexpr int MUS_EVENT_TYPE_FINISH = 6;
        constexpr int MUS_EVENT_TYPE_UNUSED = 7;

        constexpr uint8_t MUS_NOTE_VELOCITY_DEFAULT = 64;

        using utils::READ_LE_UINT16;


        const int MUSFile::MUS_PLAYBACK_SPEED_DEFAULT = 140;
        const int MUSFile::MUS_PLAYBACK_SPEED_ALTERNATE = 70;

        MUSFile::MUSFile(const std::string& filename, const int playback_speed) : File(filename),
            playback_speed(playback_speed), _header({0})
        {
            // reading similar to a standard midi.

            // now due the MUS compatibility checking:
            // 1. using 9 channels.
            // 2. files size max 64KB
            // 3. format of size is 16 bit (probably i can read from midifile directly then as it is uses 32bits)

            _assertValid(size() <= MAX_SIZE);
            readHeader();
            readTrack();
        }

        std::shared_ptr<audio::MIDI> MUSFile::getMIDI() const noexcept
        {
            return _midi;
        }

        void MUSFile::readHeader()
        {
            read(_header.id, sizeof(ID_MAGIC));
            _assertValid(strncmp(_header.id, ID_MAGIC, sizeof(ID_MAGIC)) == 0);
            _header.score_len = readLE16();
            _header.score_start = readLE16();
            _header.channels = readLE16();
            _assertValid(_header.channels <= MAX_CHANNELS);
            _header.secondary_channels = readLE16();
            //_assertValid(header.secondary_channels <= MAX_CHANNELS + 10);
            _header.instrument_counts = readLE16();
            _header.padding = readLE16();
            _assertValid(_header.padding == 0);

            instruments.reserve(_header.instrument_counts);

            // reading instruments
            instruments.reserve(_header.instrument_counts);
            for (int i = 0; i < _header.instrument_counts; i++)
                instruments.push_back(readLE16());

            _assertValid(tell() == _header.score_start);
        }

        void MUSFile::readTrack()
        {
            using audio::midi::MIDI_FORMAT;
            using audio::midi::MIDIEvent;
            using audio::midi::MIDI_EVENT_TYPES_HIGH;
            using audio::midi::MIDITrack;

            typedef union event_u
            {
                uint8_t val;
                struct e {
                    uint8_t channel : 4;
                    uint8_t type : 3;
                    uint8_t last : 1;
                } e;
            } event_u;
            static_assert(sizeof(event_u) == sizeof(uint8_t));

            MIDITrack track;
            bool quit = false;
            uint32_t delta_time = 0;
            while (!quit)
            {
                MIDIEvent me;
                event_u event;
                uint8_t d1 = 0;
                uint8_t d2 = 0;
                uint16_t pitch = 0;

                event.val = readU8();
                switch (event.e.type)
                {
                case MUS_EVENT_TYPE_RELEASE_NOTE:
                    me.type.high = static_cast<uint8_t>(MIDI_EVENT_TYPES_HIGH::NOTE_OFF);
                    d1 = readU8();
                    d2 = MUS_NOTE_VELOCITY_DEFAULT;
                    _assertValid(d1 < 128);
                    me.data.push_back(d1);
                    me.data.push_back(d2);
                    break;
                case MUS_EVENT_TYPE_PLAY_NOTE:
                    me.type.high = static_cast<uint8_t>(MIDI_EVENT_TYPES_HIGH::NOTE_ON);
                    d2 = MUS_NOTE_VELOCITY_DEFAULT;
                    d1 = readU8();
                    if ((d1 & 0x80) > 0)
                    {
                        d1 &= 0x7F;
                        d2 = readU8();
                        _assertValid((d2 & 0x80) == 0);
                    }

                    me.data.push_back(d1);
                    me.data.push_back(d2);
                    break;
                case MUS_EVENT_TYPE_PITCH_BEND:
                    me.type.high = static_cast<uint8_t>(MIDI_EVENT_TYPES_HIGH::PITCH_BEND);
                    d1 = readU8();
                    // convert to uint16_t value and mapped to +/- 2 semi-tones
                    // approximation
                    //pitch = d1 * 64;
                    pitch = d1 << 6;
                    //pitch = ((d1 & 1) >> 6) + ((d1 >> 1) & 127);
                    d1 = pitch & 0xFF;
                    d2 = pitch >> 8;
                    me.data.push_back(d1);
                    me.data.push_back(d2);
                    break;
                case MUS_EVENT_TYPE_SYS_EVENT:
                    me.type.high = static_cast<uint8_t>(MIDI_EVENT_TYPES_HIGH::CONTROLLER);
                    d1 = readU8();
                    d2 = 0;
                    _assertValid(d1 < 0x80);
                    switch (d1)
                    {
                    case 10:
                        d1 = 120;
                        break;
                    case 11:
                        d1 = 123;
                        break;
                    case 12:
                        // this might have another byte?
                        d1 = 126;
                        break;
                    case 13:
                        d1 = 127;
                        break;
                    case 14:
                        d1 = 121;
                        break;
                    case 15:
                        //break;
                    default:
                        d1 = 0;
                        break;
                    }
                    
                    if (d1 != 0) {
                        me.data.push_back(d1);
                        me.data.push_back(d2);
                    }

                    break;
                case MUS_EVENT_TYPE_CONTROLLER:
                    me.type.high = static_cast<uint8_t>(MIDI_EVENT_TYPES_HIGH::CONTROLLER);
                    d1 = readU8();
                    d2 = readU8();
                    _assertValid(d1 < 128);
                    _assertValid(d2 < 128);
                    switch (d1)
                    {
                    case 0:
                        // Change instrument, MIDI event 0xC0
                        // TODO
                        break;
                    case 1:
                        // Bank Select 0 or 32
                        // how to determine it?
                        d1 = 0;
                        break;
                    case 2:
                        // Modulation
                        d1 = 1;
                        break;
                    case 3:
                        // Volume (0=silent, ~100=normal, 127=loud)
                        d1 = 7;
                        break;
                    case 4:
                        // Pan (0=left, 64=center, 127=right)
                        d1 = 10;
                        break;
                    case 5:
                        // Expression
                        d1 = 11;
                        break;
                    case 6:
                        // Reverb depth
                        d1 = 91;
                        break;
                    case 7:
                        // Chorus depth
                        d1 = 93;
                        break;
                    case 8:
                        // Sustain pedal (hold)
                        d1 = 64;
                        break;
                    case 9:
                        // soft pedal
                        d1 = 67;
                        break;
                    default:
                        break;
                    }

                    me.data.push_back(d1);
                    me.data.push_back(d2);
                    break;
                case MUS_EVENT_TYPE_END_OF_MEASURE:
                    break;
                case MUS_EVENT_TYPE_FINISH:
                    quit = true;
                    break;
                case MUS_EVENT_TYPE_UNUSED:
                    break;
                default:
                    break;
                }

                me.type.low = event.e.channel;
                me.data.shrink_to_fit();
                me.delta_time = delta_time;
                if (me.data.size() > 0)
                    track.addEvent(me);

                if(event.e.last != 0)
                {
                    // compute delay
                    uint32_t dd = 0;
                    do {
                        d1 = readU8();
                        dd *= 128;
                        dd += d1 & 0x7F;
                        
                    } while ((d1 & 0x80) > 0);

                    delta_time = dd;
                }
            }

            track.lock();
            // division is 120BPM tempo by default so
            // 120 quarter note per minute
            // playback_speed is ticks per quarter note
            // 120 / 60 = 2 quarter notes per seconds
            // playback speed is ticks per quarter note
            // playback_speed / 2 is the division value at 120BMP
            _midi = std::make_shared<audio::MIDI>(MIDI_FORMAT::SINGLE_TRACK, 1, playback_speed / 2);
            _midi->addTrack(track);
        }
    }
}
