#include <HyperSonicDrivers/files/RIFFFile.hpp>

namespace HyperSonicDrivers::files
{
    RIFFFile::RIFFFile(
        const std::string& filename,
        const std::fstream::openmode mode) : File(filename, mode
    ) {}

    void RIFFFile::readChunkHeader(RIFF_chunk_header_t& header) const noexcept
    {
        read(&header, sizeof(RIFF_chunk_header_t));
    }

    void RIFFFile::readSubChunkHeader(RIFF_sub_chunk_header_t& header) const noexcept
    {
        read(&header, sizeof(RIFF_sub_chunk_header_t));
    }

    void RIFFFile::readId(RIFF_ID& iff_id) const noexcept
    {
        read(&iff_id, sizeof(RIFF_ID));
    }

    void RIFFFile::writeChunkHeader(const RIFF_chunk_header_t& header) noexcept
    {
        write(reinterpret_cast<const char*>(&header), sizeof(RIFF_chunk_header_t));
    }

    void RIFFFile::writeSubChunkHeader(const RIFF_sub_chunk_header_t& header) noexcept
    {
        write(reinterpret_cast<const char*>(&header), sizeof(RIFF_sub_chunk_header_t));
    }

    void RIFFFile::writeId(const RIFF_ID& iff_id) noexcept
    {
        write(reinterpret_cast<const char*>(&iff_id), sizeof(RIFF_ID));
    }

    void RIFFFile::readSubChunkHeader(const eRIFF_ID& id, RIFF_sub_chunk_header_t& s)
    {
        do {
            readSubChunkHeader(s);
            if (s.id.id != id) {
                seek(s.length, std::fstream::cur);
            }
        } while (s.id.id != id);
    }
}
