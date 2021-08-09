#include "IFFFile.hpp"
#include <stdexcept>
#include <utils/endianness.hpp>

files::IFFFile::IFFFile(const std::string& filename) : File(filename)
{
}

files::IFFFile::~IFFFile() = default;

void files::IFFFile::readChunkHeader(IFF_chunk_header_t& header)
{
	read(&header, sizeof(IFF_chunk_header_t));
	header.chunk.size = utils::read32BE(header.chunk.size);
}

void files::IFFFile::readSubChunkHeader(IFF_sub_chunk_header_t& header)
{
	read(&header, sizeof(IFF_sub_chunk_header_t));
	header.size = utils::read32BE(header.size);
}

void files::IFFFile::readId(IFF_ID& iff_id)
{
	read(&iff_id, sizeof(IFF_ID));
}
