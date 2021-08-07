#pragma once

#include "IFFFile.hpp"
#include <string>

namespace files
{
    class XMIFile final : public IFFFile
    {
    public:
        XMIFile(const std::string& filename);
        virtual ~XMIFile();

        int getNumTracks() const noexcept;
    private:
        void _readFormXdirChunk();
        void _readEvnt(const IFF_sub_chunk_header_t& IFF_evnt);
        int _num_tracks;
    };
}
