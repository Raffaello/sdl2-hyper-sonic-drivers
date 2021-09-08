#include <files/WAVFile.hpp>
#include <stdexcept>

namespace files
{
    WAVFile::WAVFile(const std::string& filename) : RIFFFile(filename)
    {
        RIFF_chunk_header_t header;

        readChunkHeader(header);
        _assertValid(header.chunk.id.id == eRIFF_ID::ID_RIFF);
        _assertValid(header.type.id == eRIFF_ID::ID_WAVE);
        
        _assertValid(header.chunk.length == size() - sizeof(RIFF_sub_chunk_header_t));
        
        // TODO read the optional chunks?
        // BODY FACT Chunk                        <fact-ck>
        // BODY Cue-Points Chunk                  <cue-ck>
        // BODY Playlist Chunk                    <playlist-ck>
        // BODY Associated Data Chunk             <assoc-data-list>


        // TODO: do it in a fmt_chunk function
        // fmt always before data chunk
        RIFF_sub_chunk_header_t fmt;
        do {
            readSubChunkHeader(fmt);
            if (fmt.id.id != eRIFF_ID::ID_FMT) {
                seek(fmt.length, std::fstream::cur);
            }
        } while (fmt.id.id != eRIFF_ID::ID_FMT);
        // <common-fields>
        _format = static_cast<eFormat>(readLE16());
        _channels = readLE16();
        _samplesPerSec = readLE32();
        _avgBytesPerSec = readLE32();
        _blockAlign = readLE16();
        // <format-specific-fields>
        switch (_format)
        {
        case eFormat::WAVE_FORMAT_PCM:
            _bitsPerSample = readLE16(); // Sample size
            break;
        case eFormat::IBM_FORMAT_MULAW:
            //break;
        case eFormat::IBM_FORMAT_ALAW:
            //break;
        case eFormat::IBM_FORMAT_ADPCM:
            //break;
        default:
            throw std::invalid_argument("WAVFile: unknown or unsupported format " + std::to_string(static_cast<int>(_format)) + " of file: " + _filename);
        }


        // TODO: do it in a data chunk function
        RIFF_sub_chunk_header_t data;
        do {
            readSubChunkHeader(data);
            if (data.id.id != eRIFF_ID::ID_DATA) {
                seek(data.length, std::fstream::cur);
            }
        } while (data.id.id != eRIFF_ID::ID_DATA);
        
        _dataSize = data.length;
        uint8_t *buf = new uint8_t[_dataSize];
        read(buf, _dataSize);
        _data.reset(buf);
    }


    WAVFile::~WAVFile()
    {
    }
}
