#include <files/MIDFile.hpp>
#include <string>
#include <utils/endianness.hpp>
#include <memory>
#include <spdlog/spdlog.h>
#include <cstring>


namespace files
{
    constexpr int32_t MAX_VRQ = 0x0FFFFFFF; // 3.5 bytes = 27 bits

    // TODO consider to "join" with IFFFile / or put common functions altogheter
    //      as it is similar to just the sub_header_chunk of IFF file.
    constexpr const char* MIDI_HEADER = "MThd";
    constexpr const char* MIDI_TRACK = "MTrk";

    constexpr int MICROSECONDS_PER_MINUTE = 60000000;

    MIDFile::MIDFile(const std::string& filename) : File(filename)
    {
        read_header();
        check_format();

        // Read Tracks
        _tracks.clear();
        _tracks.reserve(_nTracks);
        for (int i = 0; i < _nTracks; i++)
        {
            MIDI_track_t track;
            bool endTrack = false;
            MIDI_EVENT_type_u lastStatus = { 0 };
            // Read Track
            midi_chunk_t chunk = read_chunk();
            _assertValid(strncmp(chunk.id, MIDI_TRACK, sizeof(chunk.id)) == 0);
            
            // events
            int offs = 0;
            track.events.clear();
            while (!endTrack)
            {
                // MTrck Event:
                MIDI_track_event_t e;
                int debug = 0;
                // delta time encoded in VRQ
                offs += decode_VLQ(e.delta_time);
                e.type.val = readU8();
                
                if (e.type.high < 0x8) {
                    e.type = lastStatus;
                    seek(-1, std::fstream::cur);
                }
                else {
                    offs++;
                }

                switch (e.type.high)
                {
                case 0xF:
                    // special event
                    switch (e.type.low)
                    {
                    case 0x0:
                        // sys-event
                    {int a = 0; }
                        break;
                    case 0x7:
                        // sys-event
                    {int a = 0; }
                        break;
                    case 0xF:
                    {
                        // meta-event
                        uint8_t type = readU8();
                        offs++;
                        _assertValid(type < 128);
                        uint32_t length = 0;
                        offs += decode_VLQ(length);
                        e.events.reserve(length + 1);
                        e.events.push_back(type);
                        for (int j = 0; j < length; j++) {
                            e.events.push_back(readU8());
                            offs++;
                        }

                        if (MIDI_META_EVENT::END_OF_TRACK == static_cast<MIDI_META_EVENT>(type)) {
                            endTrack = true;
                        }
                        break;
                        // MOve to the sequencer/parser
                        /*
                        switch (static_cast<MIDI_META_EVENT>(type))
                        {
                        case MIDI_META_EVENT::SEQUENCE_NUMBER:
                            break;
                        case MIDI_META_EVENT::TEXT:
                            break;
                        case MIDI_META_EVENT::COPYRIGHT:
                            break;
                        case MIDI_META_EVENT::SEQUENCE_NAME:
                            break;
                        case MIDI_META_EVENT::INSTRUMENT_NAME:
                            break;
                        case MIDI_META_EVENT::LYRICS:
                            break;
                        case MIDI_META_EVENT::MARKER:
                            break;
                        case MIDI_META_EVENT::CUE_POINT:
                            break;
                        case MIDI_META_EVENT::CHANNEL_PREFIX:
                            break;
                        case MIDI_META_EVENT::MIDI_PORT:
                            buf[offs++];
                            break;
                        case MIDI_META_EVENT::END_OF_TRACK:
                            if (offs != chunk.length) {
                                spdlog::critical("out of sync reading MIDFile");
                                throw std::runtime_error("MIDFile: review the buf[offs] logic");
                            }

                            endTrack = true;
                            break;
                        case MIDI_META_EVENT::SET_TEMPO:
                            _assertValid(length == 3);

                            _tempo = (buf[offs] << 16) + (buf[offs + 1] << 8) + (buf[offs + 2]);
                            offs += length;
                            break;
                        case MIDI_META_EVENT::SMPTE_OFFSET:
                            break;
                        case MIDI_META_EVENT::TIME_SIGNATURE:
                            _numerator = buf[offs++];
                            _denominator = buf[offs++];
                            _midi_clocks_per_metronome_click = buf[offs++];
                            _bb = buf[offs++];
                            break;
                        case MIDI_META_EVENT::KEY_SIGNATURE:
                            _key = buf[offs++];
                            _scale = buf[offs++];
                            break;
                        case MIDI_META_EVENT::SEQUENCER_SPECIFIC:
                            break;
                        default:
                            spdlog::critical("MIDFile: not recognized META-EVENT {:#04x}", type);
                            throw std::runtime_error("not recognized meta-event. todo: skip");
                            break;
                        }
                        break;
                        */
                    }
                    default:
                        spdlog::critical("MIDFile sub-event {:#04x} not recognized", e.type.val);
                        throw std::runtime_error("");

                    }
                    break;
                case 0xC:
                case 0xD:
                    e.events.reserve(1);
                    e.events.push_back(readU8());
                    offs++;
                    break;
                case 0x8:
                case 0x9:
                case 0xA:
                case 0xB:
                case 0xE:
                    e.events.reserve(2);
                    e.events.push_back(readU8());
                    e.events.push_back(readU8());
                    offs += 2;
                    break;
                default:
                    // using previous status
                    if (lastStatus.val == 0) {

                        spdlog::critical("MIDFile: midi event {:#04x} not recognized {:#03x} - last status = {} (pos={}).", e.type.val, (uint8_t)e.type.high, lastStatus.val, tell());
                        throw std::runtime_error("MIDFile: midi event type not recognized.");
                    }
                }

                e.events.resize(e.events.size());
                track.events.push_back(e);
                lastStatus = e.type;
            }

            // sanity check
            if (offs != chunk.length) {
                spdlog::warn("MIDFile: FIleanme '{}' track {} length mismatch real length {}", _filename, chunk.length, offs);
            }

            _tracks.push_back(track);
        }

        _assertValid(_tracks.size() == _nTracks);
    }

    MIDFile::~MIDFile()
    {}

    int MIDFile::decode_VLQ(const uint8_t buf[], uint32_t& out_value)
    {
        int i = 0;
        uint8_t byte = 0;
        out_value = 0;
        do
        {
            if (i >= 4) {
                throw std::runtime_error("decode_VLQ: more than 32bits VLQ input");
            }

            byte = buf[i++];
            out_value = (out_value << 7) | (byte & 0x7F);
        } while (byte & 0x80);

        return i;
    }

    int MIDFile::decode_VLQ(uint32_t& out_value)
    {
        uint8_t buf[4] = { 0, 0, 0, 0 };
        uint8_t i = 0;
        uint8_t v = 0;
        
        do {
            buf[i++] = v = readU8();
            _assertValid(i < 4);
        } while (v & 0x80);
        
        return decode_VLQ(buf, out_value);
    }

    int16_t MIDFile::getFormat() const noexcept
    {
        return _format;
    }

    int16_t MIDFile::getNumTracks() const noexcept
    {
        return _nTracks;
    }

    int16_t MIDFile::getDivision() const noexcept
    {
        return _division;
    }

    const std::vector<MIDFile::MIDI_track_t>& MIDFile::getTracks() const noexcept
    {
        return _tracks;
    }

    MIDFile::midi_chunk_t MIDFile::read_chunk()
    {
        midi_chunk_t chunk;

        read(&chunk, sizeof(midi_chunk_t));
        chunk.length = utils::swapBE32(chunk.length);

        return chunk;
    }

    void MIDFile::read_header()
    {
        midi_chunk_t header = read_chunk();
        _assertValid(strncmp(header.id, MIDI_HEADER, sizeof(header.id)) == 0);
        _assertValid(header.length == 6);

        _format = readBE16();
        _nTracks = readBE16();
        _division = readBE16();
    }

    void MIDFile::check_format()
    {
        switch (static_cast<MIDI_FORMAT>(_format))
        {
        case MIDI_FORMAT::SINGLE_TRACK:
            _assertValid(_nTracks == 1);
            break;
        case MIDI_FORMAT::SIMULTANEOUS_TRACK:
        case MIDI_FORMAT::MULTI_TRACK:
            _assertValid(_nTracks >= 1);
            break;
        default:
            throw std::runtime_error("MIDFile: _format invalid");
        }
    }
}
