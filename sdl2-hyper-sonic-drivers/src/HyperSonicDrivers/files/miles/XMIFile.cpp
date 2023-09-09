#include <format>
#include <queue>
#include <bit>
#include <cassert>
#include <HyperSonicDrivers/files/miles/XMIFile.hpp>
#include <HyperSonicDrivers/audio/midi/MIDITrack.hpp>
#include <HyperSonicDrivers/utils/algorithms.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>

namespace HyperSonicDrivers::files::miles
{
    using audio::midi::MIDI_META_EVENT;
    using audio::midi::MIDIEvent;
    using audio::midi::MIDITrack;
    using utils::decode_xmi_VLQ;
    using utils::decode_VLQ;

    using utils::logD;
    using utils::logW;
    using utils::logE;
    using utils::throwLogC;

    // This first "meta-header" is optional
    // [  FORM<len>XDIR
    // {
    //     INFO<len>
    //         UWORD # of FORMs XMID in file, 1 - 65535
    // }  ]
    // ---------------------------------------------------
    // this chunk is required with at least 1 FORM with 1 EVNT
    // CAT <len>XMID
    // {
    //     FORM<len>XMID
    //         {
    //       [TIMB<len>
    //             UWORD # of timbre list entries, 0 - 16384
    //           { UBYTE patch number 0 - 127
    //             UBYTE timbre bank 0 - 127 } ...]
    // 
    //       [RBRN<len>
    //             UWORD # of branch point offsets, 0 - 127
    //           { UWORD Sequence Branch Index controller value 0 - 127
    //             ULONG controller offset from start of EVNT chunk } ...]
    // 
    //         EVNT<len>
    //           { UBYTE interval count(if < 128)
    //             UBYTE <MIDI event>(if > 127) } ...
    //         } ...
    // }
    XMIFile::XMIFile(const std::string& filename) : IFFFile(filename), File(filename)
    {
        uint16_t num_tracks = 0;
        IFF_chunk_header_t header;
        IFF_chunk_header_t cat;

        readChunkHeader(header);

        switch (header.chunk.id.id)
        {
        case eIFF_ID::ID_FORM:
            num_tracks = _readFormXdirChunk(header);
            readChunkHeader(cat);
            break;
        case eIFF_ID::ID_CAT:
            num_tracks = 1;
            cat = header;
            break;
        default:
            assertValid_(false);
        }

        assertValid_(cat.chunk.id.id == eIFF_ID::ID_CAT);
        assertValid_(cat.type.id == eIFF_ID::ID_XMID);
        assertValid_(num_tracks >= 1);

        _timbre_patch_numbers.resize(num_tracks);
        _timbre_bank.resize(num_tracks);
        // Create MIDI object
        uint16_t division = 60; // as default track tempo = 500000
        audio::midi::MIDI_FORMAT format = num_tracks == 1 ?
            audio::midi::MIDI_FORMAT::SINGLE_TRACK :
            audio::midi::MIDI_FORMAT::MULTI_TRACK;

        _midi = std::make_shared<audio::MIDI>(format, num_tracks, division);

        for (int track = 0; track < num_tracks; track++)
        {
            MIDITrack midi_track;
            IFF_chunk_header_t form_xmid;
            readChunkHeader(form_xmid);
            assertValid_(form_xmid.chunk.id.id == eIFF_ID::ID_FORM);
            assertValid_(form_xmid.type.id == eIFF_ID::ID_XMID);

            IFF_sub_chunk_header_t chunk;
            do
            {
                readSubChunkHeader(chunk);
                switch (chunk.id.id)
                {
                case eIFF_ID::ID_TIMB:
                    _readTimb(chunk, track);
                    break;
                case eIFF_ID::ID_RBRN:
                    _readRbrn(chunk, track);
                    break;
                case eIFF_ID::ID_EVNT:
                    midi_track = _readEvnts(chunk, track);
                    break;
                default:
                    std::string s(chunk.id.str, 4);
                    throw std::invalid_argument("Not a valid XMI file: " + m_filename + " (IFF_ID: " + s + ")");
                }
            } while (chunk.id.id != eIFF_ID::ID_EVNT);

            _midi->addTrack(midi_track);
        }
    }

    std::shared_ptr<audio::MIDI> XMIFile::getMIDI() const noexcept
    {
        return _midi;
    }

    uint16_t XMIFile::_readFormXdirChunk(IFF_chunk_header_t& form_xdir) const noexcept
    {
        // the FORM<len>XDIR chunk is already read and pass as a paramter
        // [  FORM<len>XDIR
        // {
        //     INFO<len>
        //         UWORD # of FORMs XMID in file, 1 - 65535
        // }  ]
        assertValid_(form_xdir.chunk.id.id == eIFF_ID::ID_FORM);
        assertValid_(form_xdir.chunk.size == sizeof(IFF_chunk_header_t) + sizeof(uint16_t));
        assertValid_(form_xdir.type.id == eIFF_ID::ID_XDIR);

        IFF_sub_chunk_header_t xdir_info;
        readSubChunkHeader(xdir_info);
        assertValid_(xdir_info.id.id == eIFF_ID::ID_INFO);
        assertValid_(xdir_info.size == sizeof(uint16_t));

        uint16_t num_tracks = readLE16();
        assertValid_(num_tracks >= 1);

        return num_tracks;
    }

    MIDITrack XMIFile::_readEvnts(const IFF_sub_chunk_header_t& IFF_evnt, const int16_t track) const noexcept
    {
        // { UBYTE interval count(if < 128)
        //     UBYTE <MIDI event>(if > 127) } ...

        using audio::midi::MIDI_EVENT_TYPES_HIGH;
        using audio::midi::MIDI_META_EVENT_TYPES_LOW;
        using audio::midi::TO_META;

        std::unique_ptr<uint8_t[]> buf = std::make_unique<uint8_t[]>(IFF_evnt.size);
        read(buf.get(), IFF_evnt.size);

        // delta time here is stored as abs_time + note duration
        auto midiEvent_cmp = [](const MIDIEvent& a, const MIDIEvent& b) {
            return a.delta_time > b.delta_time;
        };
        std::priority_queue<MIDIEvent, std::vector<MIDIEvent>, decltype(midiEvent_cmp)> notes(midiEvent_cmp);

        bool endTrack = false;
        int offs = 0;
        // there could be a risk of overflow...
        uint32_t abs_time = 0;
        MIDITrack t;
        while (!endTrack && offs < IFF_evnt.size)
        {
            MIDIEvent e;

            if (buf[offs] < 128) {
                // interval count
                offs += decode_xmi_VLQ(&buf[offs], e.delta_time);
                // check for overflow:
                assert(abs_time <= abs_time + e.delta_time);
                abs_time += e.delta_time;
            } else {
                e.delta_time = 0;
            }

            // note.delta_time is the abs_time at the moment of playing plus its duration
            // if current abs_time is >= note_delta time it means the note_off must be inserted
            // because previous delta_time was still playing.
            // TODO: at the end of the track there could be still some noteOff events
            //       to insert...
            while (!notes.empty() && e.delta_time > 0)
            {
                MIDIEvent note = notes.top();
                if (note.delta_time <= abs_time)
                {
                    notes.pop();
                    // reconstruct note_duration
                    note.delta_time = note.delta_time - (abs_time - e.delta_time);
                    // insert note off
                    t.addEvent(note);
                    // reconstruct current delta_time
                    e.delta_time -= note.delta_time;
                }
                else
                    break;
            }

            // midi event
            e.type.val = buf[offs++];
            
            // TODO: refactor later as there is quite a lot in common with MIDFile
            //       refactor when loading from a memory buffer rather then from file
            //       (when preload the whole file into memory)
            switch (static_cast<MIDI_EVENT_TYPES_HIGH>(e.type.high))
            {
            case MIDI_EVENT_TYPES_HIGH::META_SYSEX:
                // special event
                switch (static_cast<MIDI_META_EVENT_TYPES_LOW>(e.type.low))
                {
                case MIDI_META_EVENT_TYPES_LOW::SYS_EX0:
                    // sysEx-event
                    logE("sysEx 0x0 event not implemented yet");
                    break;
                case MIDI_META_EVENT_TYPES_LOW::SYS_EX7:
                    // sysEx-event
                    logE("sysEx 0x7 event not implemented yet");
                    break;
                case MIDI_META_EVENT_TYPES_LOW::META:
                {
                    // meta-event
                    uint8_t type = buf[offs++];
                    assertValid_(type < 128);
                    uint32_t length = 0;
                    offs += decode_VLQ(&buf[offs], length);
                    e.data.reserve(length + 1);
                    e.data.push_back(type);
                    for (int j = 0; j < length; j++) {
                        e.data.push_back(buf[offs++]);
                    }

                    if (MIDI_META_EVENT::END_OF_TRACK == TO_META(type))
                    {
                        endTrack = true;
                        if (offs % 2 && offs < IFF_evnt.size)
                        {
                            // 1 spare byte?
                            logD(std::format("End Of Track pad byte, valid={}", (buf[offs++] == 0)));
                        }
                    }

                    // ignore set_tempo meta event
                    /*if (MIDI_META_EVENT::SET_TEMPO == TO_META(type))
                        continue;*/
                }
                break;
                default:
                    throwLogC<std::runtime_error>(std::format("XMIFile: sub-event {:#04x} not recognized", e.type.val));
                    break;
                }
                break;
            case MIDI_EVENT_TYPES_HIGH::PROGRAM_CHANGE:
            case MIDI_EVENT_TYPES_HIGH::CHANNEL_AFTERTOUCH:
                e.data.reserve(1);
                e.data.push_back(buf[offs++]);
                break;
            case MIDI_EVENT_TYPES_HIGH::NOTE_OFF:
            {
                throwLogC<std::invalid_argument>("Note OFF event found.");
            }
            break;
            case MIDI_EVENT_TYPES_HIGH::NOTE_ON: {
                e.data.reserve(2);
                e.data.push_back(buf[offs++]);
                const uint8_t vel = buf[offs++];
                // read note duration
                MIDIEvent noteOff;
                offs += decode_VLQ(&buf[offs], noteOff.delta_time);
                if (noteOff.delta_time == 0)
                    noteOff.delta_time = 1; // TODO: is this correct?

                if (vel == 0)
                {
                    // TODO: is this ok?
                    // NOTE_OFF
                    e.type.high = static_cast<uint8_t>(MIDI_EVENT_TYPES_HIGH::NOTE_OFF);
                } 
                else
                {
                    noteOff.delta_time += abs_time;
                    noteOff.type.high = static_cast<uint8_t>(MIDI_EVENT_TYPES_HIGH::NOTE_OFF);
                    noteOff.type.low = e.type.low;
                    noteOff.data.resize(2);
                    noteOff.data[0] = e.data[0];
                    noteOff.data[1] = 40; // default if no velocity on release
                    notes.push(noteOff);
                }

                e.data.push_back(vel);
                e.data.shrink_to_fit();
            }
                break;
            case MIDI_EVENT_TYPES_HIGH::AFTERTOUCH:
            case MIDI_EVENT_TYPES_HIGH::PITCH_BEND:
            case MIDI_EVENT_TYPES_HIGH::CONTROLLER:
                e.data.reserve(2);
                e.data.push_back(buf[offs++]);
                e.data.push_back(buf[offs++]);
                break;
            default:
                throwLogC<std::runtime_error>(std::format("MIDFile: midi event {:#04x} not recognized {:#03x} - pos={}.",
                        e.type.val, std::bit_cast<uint8_t>(e.type.high), static_cast<unsigned long>(tell())));
                break;
            }

            e.data.shrink_to_fit();
            t.addEvent(e);
        }

        // sanity check
        if (offs != IFF_evnt.size)
        {
            logW(std::format("Fileanme '{}' track {} length mismatch real length {}", m_filename, IFF_evnt.size, offs));
        }

        if (!endTrack)
        {
            throwLogC<std::invalid_argument>("not a valid XMI file, missing end of track midi event");
        }

        assert(notes.empty());
        t.lock();
        return t;
    }

    void XMIFile::_readTimb(const IFF_sub_chunk_header_t& IFF_timb, const int16_t track)
    {
        // UWORD # of timbre list entries, 0 - 16384
        // { UBYTE patch number 0 - 127
        // UBYTE timbre bank 0 - 127 } ...]
        assertValid_(_timbre_patch_numbers[track].size() == 0 && _timbre_bank[track].size() == 0);
        const uint16_t timbre_list_entries = readLE16();
        assertValid_(timbre_list_entries == (IFF_timb.size - sizeof(uint16_t)) / 2);
        for (int i = 0; i < timbre_list_entries; i++)
        {
            _timbre_patch_numbers[track].push_back(readU8());
            _timbre_bank[track].push_back(readU8());
        }
        assertValid_(
            timbre_list_entries == _timbre_patch_numbers[track].size()
            && timbre_list_entries == _timbre_bank[track].size()
        );
    }
    void XMIFile::_readRbrn(const IFF_sub_chunk_header_t& IFF_rbrn, const int16_t track)
    {
        // UWORD # of branch point offsets, 0 - 127
        // { UWORD Sequence Branch Index controller value 0 - 127
        // ULONG controller offset from start of EVNT chunk } ...]
        throwLogC<std::runtime_error>("ID_RBRN not implemented yet");
    }
}
