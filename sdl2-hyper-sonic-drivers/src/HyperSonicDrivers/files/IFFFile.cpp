#include <stdexcept>
#include <HyperSonicDrivers/files/IFFFile.hpp>
#include <HyperSonicDrivers/utils/endianness.hpp>

namespace HyperSonicDrivers::files
{
   IFFFile::IFFFile(const std::string& filename) : File(filename)
    {
    }

    IFFFile::~IFFFile() = default;

    void IFFFile::readChunkHeader(IFF_chunk_header_t& header)
    {
        read(&header, sizeof(IFF_chunk_header_t));
        header.chunk.size = utils::swapBE32(header.chunk.size);
    }

    void IFFFile::readSubChunkHeader(IFF_sub_chunk_header_t& header)
    {
        read(&header, sizeof(IFF_sub_chunk_header_t));
        header.size = utils::swapBE32(header.size);
    }

    void IFFFile::readId(IFF_ID& iff_id)
    {
        read(&iff_id, sizeof(IFF_ID));
    }
}
