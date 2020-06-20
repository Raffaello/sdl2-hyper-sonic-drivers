#include "IFFFile.hpp"
#include <stdexcept>

files::IFFFile::IFFFile(const std::string& filename) : File(filename)
{	
}

files::IFFFile::~IFFFile() = default;

void files::IFFFile::readChunkHeader(IFF_chunk_header_t& header) const noexcept
{
	readOnce(&header, sizeof(IFF_chunk_header_t));
	header.size = SDL_SwapBE32(header.size);
}
