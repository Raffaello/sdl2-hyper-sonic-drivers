#include "XMIFile.hpp"
#include <string>

namespace files
{
    XMIFile::XMIFile(const std::string& filename) : IFFFile(filename)
    {
        // TODO the FORM----XDIR section is optional
        _readFormXdirChunk();

        // ---------------------------------------------------
        // this chunk is required with at least 1 FORM
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
        //_assertValid(cat.chunk.size == ???); // file size minus the previous form and 4 char?
        _assertValid(cat.type.id == eIFF_ID::ID_XMID);

        for (int form = 0; form < _num_tracks; form++)
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
                    throw std::runtime_error("ID_TIMB: not implemented yet");
                    break;
                case eIFF_ID::ID_RBRN:
                    throw std::runtime_error("ID_RBRN: not implemented yet");
                    break;
                case eIFF_ID::ID_EVNT:
                    _readEvnt(chunk);
                    break;
                default:
                    std::string s(chunk.id.str, 4);
                    throw std::invalid_argument("Not a valid XMI file: " + _filename + " (IFF_ID: " + s + ")");
                }
            } while (chunk.id.id != eIFF_ID::ID_EVNT);
        }
    }

    XMIFile::~XMIFile()
    {
    }
    int XMIFile::getNumTracks() const noexcept
    {
        return _num_tracks;
    }

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
        _assertValid(form_xdir.chunk.size == sizeof(IFF_chunk_header_t) + sizeof(IFF_sub_chunk_header_t) + sizeof(uint16_t));
        _assertValid(form_xdir.type.id == eIFF_ID::ID_XDIR);

        IFF_sub_chunk_header_t xdir_info;
        readSubChunkHeader(xdir_info);
        _assertValid(xdir_info.id.id == eIFF_ID::ID_INFO);
        _assertValid(xdir_info.size == sizeof(uint16_t));

        _num_tracks = readLE16();
        _assertValid(_num_tracks >= 1);
    }
    void XMIFile::_readEvnt(const IFF_sub_chunk_header_t& IFF_evnt)
    {
        // { UBYTE interval count(if < 128)
        //     UBYTE <MIDI event>(if > 127) } ...
        int i = 0;
        do
        {
            uint8_t value = readU8();
            i++;
            if (value < 128) {
                // interval count, a.k.a delay
                for (int j = 0; j < value; j++)
                {
                    // followed by interval count midi value???
                    uint8_t midi_value = readU8();
                }
            }
            else {
                // midi event
                uint8_t midi_value = readU8();
                i++;
            }
        } while (i < IFF_evnt.size);

        
       
        throw std::invalid_argument("IFF_EVNT: not implemented yet");
    }
}
