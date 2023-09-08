#include <HyperSonicDrivers/files/WAVFile.hpp>
#include <stdexcept>
#include <string>
//#ifdef __GNUC__
#include <cstring>
//#endif

namespace HyperSonicDrivers::files
{
    WAVFile::WAVFile(const std::string& filename, const audio::mixer::eChannelGroup group) : RIFFFile(filename),
        m_expDataChunk(false)
    {
        std::memset(&m_fmt_chunk, 0, sizeof(format_t));

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


        // fmt always before data chunk
        RIFF_sub_chunk_header_t fmt;
        do {
            readSubChunkHeader(fmt);
            if (fmt.id.id != eRIFF_ID::ID_FMT) {
                seek(fmt.length, std::fstream::cur);
            }
        } while (fmt.id.id != eRIFF_ID::ID_FMT);
        read_fmt_sub_chunk(fmt);

        RIFF_sub_chunk_header_t data;
        do {
            readSubChunkHeader(data);
            if (data.id.id != eRIFF_ID::ID_DATA) {
                seek(data.length, std::fstream::cur);
            }
        } while (data.id.id != eRIFF_ID::ID_DATA);
        read_data_sub_chunk(data);

        // TODO: works only for mono and stereo (1 or 2 channels)
        m_sound = std::make_shared<audio::Sound>(
            group,
            getFormat().channels == 2,
            getFormat().samplesPerSec,
            static_cast<uint8_t>(getFormat().bitsPerSample),
            getData()
        );
    }

    const WAVFile::format_t& WAVFile::getFormat() const noexcept
    {
        return m_fmt_chunk;
    }

    const uint32_t WAVFile::getDataSize() const noexcept
    {
        return static_cast<uint32_t>(m_data->size());
    }

    std::shared_ptr<std::vector<uint8_t>> WAVFile::getData() const noexcept
    {
        return m_data;
    }

    std::shared_ptr<audio::Sound> WAVFile::getSound() const noexcept
    {
        return m_sound;
    }

    bool WAVFile::read_fmt_sub_chunk(const RIFF_sub_chunk_header_t& chunk)
    {
        _assertValid(chunk.id.id == eRIFF_ID::ID_FMT);
        
        // <common-fields>
        m_fmt_chunk.format = static_cast<eFormat>(readLE16());
        m_fmt_chunk.channels = readLE16();
        m_fmt_chunk.samplesPerSec = readLE32();
        m_fmt_chunk.avgBytesPerSec = readLE32();
        m_fmt_chunk.blockAlign = readLE16();
        // <format-specific-fields>
        switch (m_fmt_chunk.format)
        {
        case eFormat::WAVE_FORMAT_PCM:
            m_fmt_chunk.bitsPerSample = readLE16(); // Sample size
            break;
        case eFormat::IBM_FORMAT_MULAW:
            //break;
        case eFormat::IBM_FORMAT_ALAW:
            //break;
        case eFormat::IBM_FORMAT_ADPCM:
            //break;
        default:
            throw std::invalid_argument("WAVFile: unknown or unsupported format " + std::to_string(static_cast<int>(m_fmt_chunk.format)) + " of file: " + _filename);
        }

        // fmt always before data chunk
        m_expDataChunk = true;

        return true;
    }

    bool WAVFile::read_data_sub_chunk(const RIFF_sub_chunk_header_t& chunk)
    {
        _assertValid(m_expDataChunk);
        _assertValid(chunk.id.id == eRIFF_ID::ID_DATA);

        m_data->resize(chunk.length);
        read(m_data->data(), chunk.length);
        m_data->shrink_to_fit();

        return true;
    }
}
