#include <files/dmx/MUSFile.hpp>
#include <utils/endianness.hpp>
#include <audio/midi/types.hpp>
#include <cstring>
#include <array>

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

        constexpr int MIDI_MAX_CHANNELS = 16;

        constexpr uint8_t MUS_NOTE_VELOCITY_DEFAULT = 64;

        using utils::READ_LE_UINT16;

        const int MUSFile::MUS_PLAYBACK_SPEED_DEFAULT = 140;
        const int MUSFile::MUS_PLAYBACK_SPEED_ALTERNATE = 70;

        MUSFile::MUSFile(const std::string& filename, const int playback_speed) : File(filename),
            playback_speed(playback_speed)
        {
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

            // Mapping MUS to MIDI Controls
            std::array<uint8_t,15> ctrlMap = {
                0,   // 0 program change
                0,   // 1 Bank Select
                1,   // 2 Modulation
                7,   // 3 Volume (0=silent, ~100=normal, 127=loud)
                10,  // 4 Pan (0=left, 64=center, 127=right)
                11,  // 5 Expression
                91,  // 6 Reverb Depth
                93,  // 7 Chorus Depth
                64,  // 8 Sustain Pedal (Hold)
                67,  // 9 Soft Pedal
                120, // 10
                123, // 11
                126, // 12
                127, // 13
                121, // 14
                //0    // 15
            };

            std::array<int8_t, MIDI_MAX_CHANNELS> channelMap;
            std::array<int8_t, MIDI_MAX_CHANNELS> channelVol;
            int8_t curChannel = 0;

            _assertValid(_header.channels + _header.secondary_channels < MIDI_MAX_CHANNELS);

            channelMap.fill(-1);
            channelVol.fill(64);
            // Map channel 15 to 9 (percussions)
            channelMap[15] = 9;

            MIDITrack track;
            bool quit = false;
            uint32_t delta_time = 0;
            uint32_t abs_time = 0;
            // could add the current size to end size of the file reported from the header
            // as a 2ndry check
            //tell() < (_header.score_start + _header.score_len)
            while (!quit)
            {
                MIDIEvent me;
                event_u event;
                uint8_t d1 = 0;
                uint8_t d2 = 0;

                event.val = readU8();

                if (channelMap[event.e.channel] < 0)
                {
                    // inject init channel to max volume first time that is used
                    MIDIEvent ce;

                    ce.type.high = static_cast<uint8_t>(MIDI_EVENT_TYPES_HIGH::CONTROLLER);
                    ce.type.low = curChannel;
                    ce.delta_time = delta_time;
                    ce.abs_time = abs_time;
                    ce.data.push_back(0x07); // MIDI Main Volume
                    ce.data.push_back(127);
                    track.addEvent(ce);

                    // adjust channel tracking and skip percussion if it is the case
                    channelMap[event.e.channel] = curChannel++;
                    if (curChannel == 9)
                        ++curChannel;
                }

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
                    d1 = readU8();
                    if ((d1 & 0x80) > 0)
                    {
                        d1 &= 0x7F;
                        d2 = readU8();
                        _assertValid((d2 & 0x80) == 0);
                        channelVol[channelMap[event.e.channel]] = d2;
                    }

                    d2 = channelVol[channelMap[event.e.channel]];

                    me.data.push_back(d1);
                    me.data.push_back(d2);
                    break;
                case MUS_EVENT_TYPE_PITCH_BEND:
                    me.type.high = static_cast<uint8_t>(MIDI_EVENT_TYPES_HIGH::PITCH_BEND);
                    d1 = readU8();
                    // convert to uint16_t value and mapped to +/- 2 semi-tones
                    d2 = d1;
                    d1 = (d1 & 1) >> 6; // isn't it always 0 ?
                    d2 = (d2 >> 1) & 127;
                    me.data.push_back(d1);
                    me.data.push_back(d2);
                    break;
                case MUS_EVENT_TYPE_SYS_EVENT:
                    me.type.high = static_cast<uint8_t>(MIDI_EVENT_TYPES_HIGH::CONTROLLER);
                    d1 = readU8();
                    d2 = readU8() == 12 ? static_cast<uint8_t>(_header.channels + 1) : 0; // ?
                    
                    _assertValid(d1 < 0x80);
                    
                    me.data.push_back(ctrlMap.at(d1));
                    me.data.push_back(d2);
                    break;
                case MUS_EVENT_TYPE_CONTROLLER:
                    d1 = readU8();
                    d2 = readU8();
                    _assertValid(d1 < 128);
                    _assertValid(d2 < 128);

                    if (d1 == 0)
                    {
                        // Change instrument, MIDI event 0xC0
                        me.type.high = static_cast<uint8_t>(MIDI_EVENT_TYPES_HIGH::PROGRAM_CHANGE);
                        me.data.push_back(d2);
                    }
                    else {
                        // Controller event
                        me.type.high = static_cast<uint8_t>(MIDI_EVENT_TYPES_HIGH::CONTROLLER);
                        me.data.push_back(ctrlMap.at(d1));
                        me.data.push_back(d2);
                    }
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

                me.type.low = channelMap[event.e.channel];
                me.data.shrink_to_fit();
                me.delta_time = delta_time;
                me.abs_time = abs_time;
                if (!me.data.empty())
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
                    abs_time += delta_time;
                }
                else
                    delta_time = 0;
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
