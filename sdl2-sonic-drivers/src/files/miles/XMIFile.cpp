#include <files/miles/XMIFile.hpp>
#include <string>

namespace files
{
    namespace miles
    {
        XMIFile::XMIFile(const std::string& filename) : IFFFile(filename)
        {
            IFF_ID xmiId;
            readId(xmiId);
            // TODO remove this seek stmt, as redundant
            seek(0, std::fstream::_Seekbeg);
            switch (xmiId.id)
            {
            case eIFF_ID::ID_FORM:
                _readFormXdirChunk();
                break;
            case eIFF_ID::ID_CAT:
                _num_tracks = 1;
                break;
            default:
                _assertValid(false);
            }

            _midi_events.resize(_num_tracks);
            _timbre_patch_numbers.resize(_num_tracks);
            _timbre_bank.resize(_num_tracks);
            
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
            IFF_chunk_header_t cat;
            readChunkHeader(cat);
            _assertValid(cat.chunk.id.id == eIFF_ID::ID_CAT);
            _assertValid(cat.type.id == eIFF_ID::ID_XMID);

            for (int track = 0; track < _num_tracks; track++)
            {
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
                        _readEvnt(chunk, track);
                        break;
                    default:
                        std::string s(chunk.id.str, 4);
                        throw std::invalid_argument("Not a valid XMI file: " + _filename + " (IFF_ID: " + s + ")");
                    }
                } while (chunk.id.id != eIFF_ID::ID_EVNT);
            }


            // Create MIDI object
            /*audio::midi::MIDI_FORMAT format = _num_tracks == 1 ?
                audio::midi::MIDI_FORMAT::SINGLE_TRACK :
                audio::midi::MIDI_FORMAT::MULTI_TRACK;

            _midi = std::make_shared<audio::MIDI>(format, _num_tracks, division);*/
        }

        XMIFile::~XMIFile()
        {
        }

        /*int XMIFile::getNumTracks() const noexcept
        {
            return _num_tracks;
        }*/

        /*const std::vector<uint8_t>& XMIFile::getTrack(const uint16_t track) const noexcept
        {
            return _midi_events[track];
        }*/

        void XMIFile::_readFormXdirChunk()
        {
            // ---------------------------------------------------
            // [  FORM<len>XDIR
            // {
            //     INFO<len>
            //         UWORD # of FORMs XMID in file, 1 - 65535
            // }  ]
            IFF_chunk_header_t form_xdir;

            readChunkHeader(form_xdir);
            _assertValid(form_xdir.chunk.id.id == eIFF_ID::ID_FORM);
            _assertValid(form_xdir.chunk.size == sizeof(IFF_chunk_header_t) + sizeof(uint16_t));
            _assertValid(form_xdir.type.id == eIFF_ID::ID_XDIR);

            IFF_sub_chunk_header_t xdir_info;
            readSubChunkHeader(xdir_info);
            _assertValid(xdir_info.id.id == eIFF_ID::ID_INFO);
            _assertValid(xdir_info.size == sizeof(uint16_t));

            _num_tracks = readLE16();
            _assertValid(_num_tracks >= 1);
        }

        void XMIFile::_readEvnt(const IFF_sub_chunk_header_t& IFF_evnt, const int16_t track)
        {
            // TODO: midi events need to be "parsed"
            // body: like for MIDFile to be validated
            // body: and construct the MIDI object.
            // body: what is needed at least is the 
            // body: division, value.
            // body: also it requires to be 
            // body: created to create MIDIEvents objecys
            // body: and MIDITrack objects
            // body: to be added in MIDI object.
            

            // { UBYTE interval count(if < 128)
            //     UBYTE <MIDI event>(if > 127) } ...
            _assertValid(_midi_events[track].size() == 0);
            for (int i = 0; i < IFF_evnt.size; i++) {
                _midi_events[track].push_back(readU8());
            }
            _assertValid(_midi_events[track].size() == IFF_evnt.size);
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
