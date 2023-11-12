#include <HyperSonicDrivers/files/WAVFile.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>
#include <format>
#include <string>
#include <memory>
#include <stdexcept>
//#ifdef __GNUC__
#include <cstring>
//#endif

namespace HyperSonicDrivers::files
{
        WAVFile::WAVFile(
        const std::string& filename,
        const audio::mixer::eChannelGroup group,
        const bool read_mode) :
        RIFFFile(
            filename,
            read_mode ?
            std::fstream::in | std::fstream::binary :
            std::fstream::out | std::fstream::binary
        ),
        m_read_mode(read_mode)
    {
        std::memset(&m_fmt_chunk, 0, sizeof(format_t));
        
        if (!read_mode)
            return;

        RIFF_chunk_header_t header;
        readChunkHeader(header);
        assertValid_(header.chunk.id.id == eRIFF_ID::ID_RIFF);
        assertValid_(header.type.id == eRIFF_ID::ID_WAVE);
        assertValid_(header.chunk.length == size() - sizeof(RIFF_sub_chunk_header_t));

        // TODO read the optional chunks?
        // BODY FACT Chunk                        <fact-ck>
        // BODY Cue-Points Chunk                  <cue-ck>
        // BODY Playlist Chunk                    <playlist-ck>
        // BODY Associated Data Chunk             <assoc-data-list>


        // fmt always before data chunk
        RIFF_sub_chunk_header_t fmt;
        readSubChunkHeader(eRIFF_ID::ID_FMT, fmt);
        read_fmt_sub_chunk(fmt);

        RIFF_sub_chunk_header_t data;
        readSubChunkHeader(eRIFF_ID::ID_DATA, data);
        read_data_sub_chunk(data);

        make_pcm_sound_(group);
    }

    WAVFile::~WAVFile()
    {
        if (!m_read_mode && m_isSaving)
            save_end();
    }

    const WAVFile::format_t& WAVFile::getFormat() const noexcept
    {
        return m_fmt_chunk;
    }

    void WAVFile::save_prepare(const uint32_t freq, const bool stereo)
    {
        if (m_isSaving)
            return;

        RIFF_chunk_header_t header;
        header.chunk.id.id = eRIFF_ID::ID_RIFF;
        header.chunk.length = static_cast<uint32_t>(m_saving_data_length_pos); // empty wav file
        header.type.id = eRIFF_ID::ID_WAVE;
        writeChunkHeader(header); // 12 bytes

        // FMT chunk
        RIFF_sub_chunk_header_t sub_header;
        sub_header.id.id = eRIFF_ID::ID_FMT;
        sub_header.length = sizeof(format_t);
        writeSubChunkHeader(sub_header); // 20 bytes

        // writing FMT data (PCM, 2 extra byte in FMT data)
        m_fmt_chunk.format = eFormat::WAVE_FORMAT_PCM;
        m_fmt_chunk.channels = stereo ? 2 : 1;
        m_fmt_chunk.samplesPerSec = freq;
        m_fmt_chunk.blockAlign = sizeof(int16_t) * m_fmt_chunk.channels;
        m_fmt_chunk.avgBytesPerSec = freq * m_fmt_chunk.blockAlign;
        m_fmt_chunk.bitsPerSample = 8 * sizeof(int16_t);
        write(reinterpret_cast<const char*>(&m_fmt_chunk), sizeof(format_t)); // 36 bytes

        // DATA chunk
        sub_header.id.id = eRIFF_ID::ID_DATA;
        sub_header.length = 0;
        writeSubChunkHeader(sub_header); // 44 bytes

        m_isSaving = true;
    }

    void WAVFile::save_streaming(const int16_t* buffer, const size_t length)
    {
        if (!m_isSaving)
            return;

        if (length == 0 || buffer == nullptr)
            return;

        write(reinterpret_cast<const char*>(buffer), length * (sizeof(int16_t) / sizeof(char)));
    }

    void WAVFile::save_end()
    {
        if (!m_isSaving)
            return;

        flush();
        if (size() % 2 == 1)
        {
            const char pad = 0;
            write(&pad, sizeof(char));
            flush();
        }

        const auto size = this->size();
        const uint32_t length = static_cast<uint32_t>(size - m_saving_data_length_pos - sizeof(uint32_t)); // extra PCM_FORMAT field (2 bytes) (if present)
        const uint32_t size4 = static_cast<uint32_t>(size - sizeof(RIFF_sub_chunk_header_t));

        seek(m_saving_length_pos);
        write(reinterpret_cast<const char*>(&size4), sizeof(uint32_t));
        seek(m_saving_data_length_pos);
        write(reinterpret_cast<const char*>(&length), sizeof(uint32_t));
        flush();
        m_isSaving = false;
    }

    void WAVFile::save(const uint32_t freq, const bool stereo, const int16_t* sound, const size_t length)
    {
        save_prepare(freq, stereo);
        save_streaming(sound, length);
        save_end();
    }

    void WAVFile::save(const audio::PCMSound& sound)
    {
        save(sound.freq, sound.stereo, sound.data.get(), sound.dataSize);
    }

    bool WAVFile::read_fmt_sub_chunk(const RIFF_sub_chunk_header_t& chunk)
    {
        assertValid_(chunk.id.id == eRIFF_ID::ID_FMT);
        
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
            throw std::invalid_argument("WAVFile: unknown or unsupported format " + std::to_string(static_cast<int>(m_fmt_chunk.format)) + " of file: " + m_filename);
        }

        // fmt always before data chunk
        m_expDataChunk = true;
        bitsDepth = static_cast<uint8_t>(m_fmt_chunk.bitsPerSample);
        m_channels = m_fmt_chunk.channels;
        freq = m_fmt_chunk.samplesPerSec;

        return true;
    }

    bool WAVFile::read_data_sub_chunk(const RIFF_sub_chunk_header_t& chunk)
    {
        assertValid_(m_expDataChunk);
        assertValid_(chunk.id.id == eRIFF_ID::ID_DATA);

        m_dataSize = chunk.length;
        m_data = std::make_shared<uint8_t[]>(m_dataSize);
        read(m_data.get(), m_dataSize);

        return true;
    }
}
