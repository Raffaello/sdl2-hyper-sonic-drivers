#include <files/RIFFFile.hpp>

namespace HyperSonicDrivers::files
{
    RIFFFile::RIFFFile(const std::string& filename) : File(filename)
    {
    }

    RIFFFile::~RIFFFile() = default;

    void RIFFFile::readChunkHeader(RIFF_chunk_header_t& header)
    {
        read(&header, sizeof(RIFF_chunk_header_t));
    }
    void RIFFFile::readSubChunkHeader(RIFF_sub_chunk_header_t& header)
    {
        read(&header, sizeof(RIFF_sub_chunk_header_t));
    }
    void RIFFFile::readId(RIFF_ID& iff_id)
    {
        read(&iff_id, sizeof(RIFF_ID));
    }
}
