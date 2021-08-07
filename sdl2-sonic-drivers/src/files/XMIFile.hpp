#pragma once

#include <files/IFFFile.hpp>
#include <string>
#include <vector>

namespace files
{
    class XMIFile final : public IFFFile
    {
    public:
        XMIFile(const std::string& filename);
        virtual ~XMIFile();

        int getNumTracks() const noexcept;
        const std::vector<uint8_t>& getTrack(const uint16_t track) const noexcept;
    private:
        void _readFormXdirChunk();
        void _readEvnt(const IFF_sub_chunk_header_t& IFF_evnt, const int16_t track);
        int _num_tracks;
        std::vector<std::vector<uint8_t>> _midi_events;
    };
}
