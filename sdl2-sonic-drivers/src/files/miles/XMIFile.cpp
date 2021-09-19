#include <files/miles/XMIFile.hpp>
#include <audio/midi/MIDITrack.hpp>
#include <utils/algorithms.hpp>
#include <spdlog/spdlog.h>

namespace files
{
    namespace miles
    {
        using audio::midi::MIDI_META_EVENT;
        using audio::midi::MIDIEvent;
        using audio::midi::MIDITrack;
        using utils::decode_xmi_VLQ;
        using utils::decode_VLQ;

        

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
        XMIFile::XMIFile(const std::string& filename) : IFFFile(filename)
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
                _assertValid(false);
            }

            _assertValid(cat.chunk.id.id == eIFF_ID::ID_CAT);
            _assertValid(cat.type.id == eIFF_ID::ID_XMID);
            _assertValid(num_tracks >= 1);

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
                _assertValid(form_xmid.chunk.id.id == eIFF_ID::ID_FORM);
                _assertValid(form_xmid.type.id == eIFF_ID::ID_XMID);
                
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
                        throw std::invalid_argument("Not a valid XMI file: " + _filename + " (IFF_ID: " + s + ")");
                    }
                } while (chunk.id.id != eIFF_ID::ID_EVNT);

                _midi->addTrack(midi_track);
            }
        }

        XMIFile::~XMIFile()
        {
        }

        std::shared_ptr<audio::MIDI> XMIFile::getMIDI() const noexcept
        {
            return _midi;
        }

        uint16_t XMIFile::_readFormXdirChunk(IFF_chunk_header_t& form_xdir)
        {
            // ---------------------------------------------------
            // [  FORM<len>XDIR
            // {
            //     INFO<len>
            //         UWORD # of FORMs XMID in file, 1 - 65535
            // }  ]
            _assertValid(form_xdir.chunk.id.id == eIFF_ID::ID_FORM);
            _assertValid(form_xdir.chunk.size == sizeof(IFF_chunk_header_t) + sizeof(uint16_t));
            _assertValid(form_xdir.type.id == eIFF_ID::ID_XDIR);

            IFF_sub_chunk_header_t xdir_info;
            readSubChunkHeader(xdir_info);
            _assertValid(xdir_info.id.id == eIFF_ID::ID_INFO);
            _assertValid(xdir_info.size == sizeof(uint16_t));

            uint16_t num_tracks = readLE16();
            _assertValid(num_tracks >= 1);

            return num_tracks;
        }

        MIDITrack XMIFile::_readEvnts(const IFF_sub_chunk_header_t& IFF_evnt, const int16_t track)
        {
            // { UBYTE interval count(if < 128)
            //     UBYTE <MIDI event>(if > 127) } ...
            
            std::unique_ptr<uint8_t[]> buf = std::make_unique<uint8_t[]>(IFF_evnt.size);
            read(buf.get(), IFF_evnt.size);

            bool endTrack = false;
            int offs = 0;
            MIDITrack t;

            while(!endTrack && offs < IFF_evnt.size)
            {
                MIDIEvent e;

                offs += decode_xmi_VLQ(&buf[offs], e.delta_time);
                e.type.val = buf[offs++];
                // TODO: refactor later as there is quite a lot in common with MIDFile
                switch (e.type.high)
                {
                case 0xF:
                    // special event
                    switch (e.type.low)
                    {
                    case 0x0:
                        // sysEx-event
                        spdlog::error("sysEx 0x0 event not implemented yet");
                        break;
                    case 0x7:
                        // sysEx-event
                        spdlog::error("sysEx 0x7 event not implemented yet");
                        break;
                    case 0xF:
                    {
                        // meta-event
                        uint8_t type = buf[offs++];
                        _assertValid(type < 128);
                        uint32_t length = 0;
                        offs += decode_VLQ(&buf[offs], length);
                        e.data.reserve(length + 1);
                        e.data.push_back(type);
                        for (int j = 0; j < length; j++) {
                            e.data.push_back(buf[offs++]);
                        }

                        if (MIDI_META_EVENT::END_OF_TRACK == static_cast<MIDI_META_EVENT>(type))
                        {
                            endTrack = true;
                            if (offs % 2 && offs < IFF_evnt.size) {
                                // 1 spare byte?
                                spdlog::debug("End Of Track pad byte, valid={}", (bool)buf[offs++] == 0);
                            }
                        }
                        break;
                    }
                    default:
                        spdlog::critical("XMIFile: sub-event {:#04x} not recognized", e.type.val);
                        throw std::runtime_error("");

                    }
                    break;
                case 0xC:
                case 0xD:
                    e.data.reserve(1);
                    e.data.push_back(buf[offs++]);
                    break;
                case 0x8: // Note OFF
                {
                    const char* err_msg = "Note OFF event found.";
                    spdlog::critical(err_msg);
                    throw std::invalid_argument(err_msg);
                }
                break;
                case 0x9: // Note ON
                    e.data.reserve(3);
                    e.data.push_back(buf[offs++]);
                    e.data.push_back(buf[offs++]);
                    e.data.push_back(buf[offs++]);
                    break;
                case 0xA:
                case 0xB:
                case 0xE:
                    e.data.reserve(2);
                    e.data.push_back(buf[offs++]);
                    e.data.push_back(buf[offs++]);
                    break;
                default:
                    spdlog::critical("MIDFile: midi event {:#04x} not recognized {:#03x} - pos={}.", e.type.val, (uint8_t)e.type.high, tell());
                    throw std::runtime_error("XMIFile: midi event type not recognized.");
                }

                e.data.resize(e.data.size());
                t.addEvent(e);
            }

            

            // sanity check
            if (offs != IFF_evnt.size) {
                spdlog::warn("XMIFile: Fileanme '{}' track {} length mismatch real length {}", _filename, IFF_evnt.size, offs);
            }

            if (!endTrack) {
                const char* err_msg = "XMIFile: not a valid XMI file, missing end of track midi event";
                spdlog::critical(err_msg);
                throw std::invalid_argument(err_msg);
            }
            
        
            return t;
        }

        void XMIFile::_readTimb(const IFF_sub_chunk_header_t& IFF_timb, const int16_t track)
        {
            //             UWORD # of timbre list entries, 0 - 16384
            //           { UBYTE patch number 0 - 127
            //             UBYTE timbre bank 0 - 127 } ...]
            _assertValid(_timbre_patch_numbers[track].size() == 0 && _timbre_bank[track].size() == 0);
            const uint16_t timbre_list_entries = readLE16();
            _assertValid(timbre_list_entries == (IFF_timb.size - sizeof(uint16_t)) / 2);
            for (int i = 0; i < timbre_list_entries; i++)
            {
                _timbre_patch_numbers[track].push_back(readU8());
                _timbre_bank[track].push_back(readU8());
            }
            _assertValid(
                timbre_list_entries == _timbre_patch_numbers[track].size()
                && timbre_list_entries == _timbre_bank[track].size()
            );
        }
        void XMIFile::_readRbrn(const IFF_sub_chunk_header_t& IFF_rbrn, const int16_t track)
        {
            //             UWORD # of branch point offsets, 0 - 127
            //           { UWORD Sequence Branch Index controller value 0 - 127
            //             ULONG controller offset from start of EVNT chunk } ...]
            throw std::runtime_error("ID_RBRN: not implemented yet");
        }
    }
}
