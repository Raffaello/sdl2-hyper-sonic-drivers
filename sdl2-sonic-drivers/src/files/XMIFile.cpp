#include "XMIFile.hpp"
#include <string>

namespace files
{
    XMIFile::XMIFile(const std::string& filename) : IFFFile(filename)
    {
        IFF_chunk_header_t header;
        readChunkHeader(header);
        _assertValid(header.id.id == eIFF_ID::ID_FORM);
        
        IFF_chunk_header_t xdir;
        readChunkHeader(xdir);
        _assertValid(xdir.id.id == eIFF_ID::ID_XDIR);

        seek(-sizeof(xdir.size), RW_SEEK_CUR);
        IFF_chunk_header_t xdir_info;
        readChunkHeader(xdir_info);
        _assertValid(xdir_info.id.id == eIFF_ID::ID_INFO);
        _songs = readLE16();
        // ---- header.size --- point here
        for (int song = 0; song < _songs; song++) {
            IFF_chunk_header_t cat;
            //readChunkHeader(cat);
            //_assertValid(cat.id.id == eIFF_ID::ID_CAT);

            // todo...

            // fix IFF on dune2 dumper first.
            // then come back here
        }

       
    }

    XMIFile::~XMIFile()
    {
    }
}
