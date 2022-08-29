#include <files/dmx/MUSFile.hpp>
#include <utils/endianness.hpp>
#include <audio/midi/types.hpp>
#include <cstring>
#include <array>

#include <spdlog/spdlog.h>

namespace files
{
    namespace dmx
    {
        constexpr const char ID_MAGIC[4] = { 'M','U','S','\x1A' };
        constexpr int MAX_SIZE = 1024 * 64;
        
        constexpr int MUS_PRIMARY_CHANNELS = 9; // OPL2 9, OPL3 18 instead
        constexpr int MUS_SECONDARY_CHANNLES = 16 - MUS_PRIMARY_CHANNELS;
        constexpr int MUS_PERCURSION_CHANNEL = 15;
        constexpr int MUS_EVENT_TYPE_RELEASE_NOTE = 0;
        constexpr int MUS_EVENT_TYPE_PLAY_NOTE = 1;
        constexpr int MUS_EVENT_TYPE_PITCH_BEND = 2;
        constexpr int MUS_EVENT_TYPE_SYS_EVENT = 3;
        constexpr int MUS_EVENT_TYPE_CONTROLLER = 4;
        constexpr int MUS_EVENT_TYPE_END_OF_MEASURE = 5;
        constexpr int MUS_EVENT_TYPE_FINISH = 6;
        constexpr int MUS_EVENT_TYPE_UNUSED = 7;
        constexpr uint8_t MUS_NOTE_VELOCITY_DEFAULT = 64;

        using audio::midi::MIDI_MAX_CHANNELS;
        using audio::midi::MIDI_PERCUSSION_CHANNEL;

        using utils::READ_LE_UINT16;

        const int MUSFile::MUS_PLAYBACK_SPEED_DEFAULT = 140;
        const int MUSFile::MUS_PLAYBACK_SPEED_ALTERNATE = 70;

        const std::array<uint8_t, 15> MUSFile::ctrlMap = {
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
                120, // 10 all sounds off
                123, // 11 all notes off
                126, // 12 mono
                127, // 13 poly
                121, // 14 reset all controllers
                //0    // 15
        };

        MUSFile::MUSFile(const std::string& filename, const int playback_speed) : File(filename),
            playback_speed(playback_speed)
        {
            _assertValid(size() <= MAX_SIZE);
            readHeader();
            readTrack();
        }

        std::shared_ptr<audio::MIDI> MUSFile::getMIDI() noexcept
        {
            return convertToMidi();
        }

        void MUSFile::readHeader()
        {
            read(_header.id, sizeof(ID_MAGIC));
            _assertValid(strncmp(_header.id, ID_MAGIC, sizeof(ID_MAGIC)) == 0);
            _header.score_len = readLE16();
            _header.score_start = readLE16();
            _header.channels = readLE16();
            _assertValid(_header.channels <= MUS_PRIMARY_CHANNELS);
            _header.secondary_channels = readLE16();
            _assertValid(_header.secondary_channels <= MUS_SECONDARY_CHANNLES);
            _header.instrument_counts = readLE16();
            _header.padding = readLE16();
            _assertValid(_header.padding == 0);

            instruments.reserve(_header.instrument_counts);

            // reading instruments
            instruments.reserve(_header.instrument_counts);
            for (int i = 0; i < _header.instrument_counts; i++)
                instruments.push_back(readLE16());
            
            instruments.shrink_to_fit();
            _assertValid(_header.channels + _header.secondary_channels < MIDI_MAX_CHANNELS);
        }

        void MUSFile::readTrack()
        {
            _assertValid(tell() == _header.score_start);

            std::array<int8_t, MIDI_MAX_CHANNELS> channelVol;
            channelVol.fill(127);
            bool quit = false;
            uint32_t delta_time = 0;
            
            while (!quit)
            {
                mus_event_t event;
                uint8_t d1 = 0;
                uint8_t d2 = 0;

                event.desc.val = readU8();
                switch (event.desc.e.type)
                {
                case MUS_EVENT_TYPE_RELEASE_NOTE:
                    d1 = readU8();
                    _assertValid(d1 < 128);
                    event.data.push_back(d1);
                    break;
                case MUS_EVENT_TYPE_PLAY_NOTE:
                    d1 = readU8();
                    if ((d1 & 0x80) > 0)
                    {
                        d1 &= 0x7F;
                        d2 = readU8();
                        _assertValid((d2 & 0x80) == 0);
                        channelVol[event.desc.e.channel] = d2;
                    }
                    d2 = channelVol[event.desc.e.channel];
                    event.data.push_back(d1);
                    event.data.push_back(d2);
                    break;
                case MUS_EVENT_TYPE_PITCH_BEND:
                    d1 = readU8();
                    event.data.push_back(d1);
                    break;
                case MUS_EVENT_TYPE_SYS_EVENT:
                    d1 = readU8();
                    _assertValid(d1 < 0x80);
                    event.data.push_back(d1);
                    break;
                case MUS_EVENT_TYPE_CONTROLLER:
                    d1 = readU8();
                    d2 = readU8();
                    _assertValid(d1 < 128); // bit 2^7 always 0
                    _assertValid(d2 < 128); // bit 2^7 always 0

                    event.data.push_back(d1);
                    event.data.push_back(d2);
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

                event.data.shrink_to_fit();
                event.delta_time = delta_time;
                if (!event.data.empty())
                    _mus.push_back(event);

                if(event.desc.e.last != 0)
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
                else
                    delta_time = 0;
            }
        }

        std::shared_ptr<audio::MIDI> MUSFile::convertToMidi()
        {
            using audio::midi::MIDI_FORMAT;
            using audio::midi::MIDIEvent;
            using audio::midi::MIDI_EVENT_TYPES_HIGH;
            using audio::midi::MIDITrack;

            std::array<bool, MIDI_MAX_CHANNELS> channelInit;

            channelInit.fill(false);

            MIDITrack track;
            uint32_t delta_time = 0;
            uint32_t abs_time = 0;

            channelInit[MUS_PERCURSION_CHANNEL] = true;
            for (auto& event : _mus)
            {
                MIDIEvent me;
                uint8_t d1 = 0;
                uint8_t d2 = 0;

                if(!channelInit[event.desc.e.channel])
                {
                    // inject init channel to max volume first time that is used
                    MIDIEvent ce;

                    ce.type.high = static_cast<uint8_t>(MIDI_EVENT_TYPES_HIGH::CONTROLLER);
                    ce.type.low = event.desc.e.channel;
                    ce.delta_time = delta_time;
                    ce.abs_time = abs_time;
                    ce.data.push_back(0x07); // MIDI Channel (a.k.a. Main) Volume
                    ce.data.push_back(127);
                    track.addEvent(ce);

                    // inited
                    channelInit[event.desc.e.channel] = true;
                }

                switch (event.desc.e.type)
                {
                case MUS_EVENT_TYPE_RELEASE_NOTE:
                    me.type.high = static_cast<uint8_t>(MIDI_EVENT_TYPES_HIGH::NOTE_OFF);
                    me.data.push_back(event.data[0]);
                    me.data.push_back(MUS_NOTE_VELOCITY_DEFAULT);
                    break;
                case MUS_EVENT_TYPE_PLAY_NOTE:
                    me.type.high = static_cast<uint8_t>(MIDI_EVENT_TYPES_HIGH::NOTE_ON);
                    me.data.push_back(event.data[0]);
                    me.data.push_back(event.data[1]);
                    break;
                case MUS_EVENT_TYPE_PITCH_BEND:
                {
                    me.type.high = static_cast<uint8_t>(MIDI_EVENT_TYPES_HIGH::PITCH_BEND);
                    // convert to uint16_t value and mapped to +/- 2 semi-tones (14 bits number to be divided)
                    uint16_t bend = event.data[0] << 6;
                    d1 = (bend & 0x7F);
                    d2 = (bend >> 7) & 0x7F;
                    me.data.push_back(d1);
                    me.data.push_back(d2);
                    break;
                }
                case MUS_EVENT_TYPE_SYS_EVENT:
                    me.type.high = static_cast<uint8_t>(MIDI_EVENT_TYPES_HIGH::CONTROLLER);
                    me.data.push_back(ctrlMap.at(event.data[0]));
                    me.data.push_back(0);
                    break;
                case MUS_EVENT_TYPE_CONTROLLER:
                    d1 = event.data[0];
                    d2 = event.data[1];

                    if (d1 == 0)
                    {
                        // NOTE:
                        // Here changes the instrument type, need to preload GENMIDI.OP2?
                        // in the Midi drv? as OPL as 4 voices and during a song
                        // the instruments may change, therefore need to load the instrument patch
                        // when it is played.
                        // How can be done instead a preload of all instrument with MIDI and send,
                        // the instrument to play to the adlib channel? 
                        // need to do through a "MIDI AdLib Driver" ...
                        // So not sure now.. i should read a MUS file normally then doing in midi

                        // Change instrument, MIDI event 0xC0
                        me.type.high = static_cast<uint8_t>(MIDI_EVENT_TYPES_HIGH::PROGRAM_CHANGE);
                        me.data.push_back(d2);
                        //auto d3 = instruments[event.desc.e.channel];
                        /*if (d2 != d3) {
                            spdlog::error("instrument d2 {:d} => d3 {:d}", d2, d3);
                        }*/
                    }
                    else {
                        // Controller event
                        me.type.high = static_cast<uint8_t>(MIDI_EVENT_TYPES_HIGH::CONTROLLER);
                        me.data.push_back(ctrlMap.at(d1));
                        me.data.push_back(d2);
                    }
                    break;
                default:
                    break;
                }

                delta_time = event.delta_time;
                abs_time += delta_time;

                // swap percurssion channel with OPL2 percussion channel
                switch (event.desc.e.channel) {
                case MUS_PERCURSION_CHANNEL:
                    me.type.low = MIDI_PERCUSSION_CHANNEL;
                    break;
                case MIDI_PERCUSSION_CHANNEL:
                    me.type.low = MUS_PERCURSION_CHANNEL;
                    break;
                default:
                    me.type.low = event.desc.e.channel;
                    break;
                }

                me.data.shrink_to_fit();
                me.delta_time = delta_time;
                me.abs_time = abs_time;
                if (!me.data.empty())
                    track.addEvent(me);
            }

            track.lock();
            // division is 120BPM tempo by default so
            // 120 quarter note per minute
            // playback_speed is ticks per quarter note
            // 120 / 60 = 2 quarter notes per seconds
            // playback speed is ticks per quarter note
            // playback_speed / 2 is the division value at 120BMP
            std::shared_ptr<audio::MIDI> midi;
            midi = std::make_shared<audio::MIDI>(MIDI_FORMAT::SINGLE_TRACK, 1, playback_speed / 2);
            midi->addTrack(track);

            return midi;
        }
    }
}
