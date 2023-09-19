#pragma once

#include <HyperSonicDrivers/audio/Sound.hpp>
#include <HyperSonicDrivers/audio/mixer/ChannelGroup.hpp>
#include <HyperSonicDrivers/files/RIFFFile.hpp>
#include <HyperSonicDrivers/files/IPCMFile.hpp>
#include <string>
#include <memory>

namespace HyperSonicDrivers::files
{
    class WAVFile final : protected RIFFFile, public IPCMFile
    {
    public:
        enum class eFormat : uint16_t
        {
            WAVE_FORMAT_PCM            = 0x0001,
            WAVE_FORMAT_IEEE_FLOAT     = 0x0003,
            WAVE_FORMAT_ALAW           = 0x0006,
            WAVE_FORMAT_MULAW          = 0x0007,
            IBM_FORMAT_MULAW           = 0x0101,
            IBM_FORMAT_ALAW            = 0x0102,
            IBM_FORMAT_ADPCM           = 0x0103,
            WAVE_FORMAT_CREATIVE_ADPCM = 0x0200,
            WAVE_FORMAT_EXTENSIBLE     = 0xFFFE,
            WAVE_FORMAT_DRM = 0,
            WAVE_FORMAT_ADPCM = 0
        };

        // TODO: consider remove eFormat field or make it adaptable
        // BODY: can be read at once otherwise...
        // BODY: could have a private format_1 struct
        // BODY: to copy over....
        // BODY: (PCM fmt size 16 bytes, non-PCM 18 bytes, float 40 bytes)
        // BODY: can be split in format_common struct
        // BODY: and in the specific format structures.
        // BODY: dynamically allocated
        typedef struct format_t
        {
            eFormat  format;         // must be WAVE_FORMAT_PCM
            uint16_t channels;
            uint32_t samplesPerSec;
            uint32_t avgBytesPerSec;
            uint16_t blockAlign;
            uint16_t bitsPerSample; // only for format WAVE_FORMAT_PCM (PCM)
        } format_t;
        static_assert(2 + 2 + 4 + 4 + 2 + 2 == sizeof(format_t));

        WAVFile(
            const std::string& filename,
            const audio::mixer::eChannelGroup group = audio::mixer::eChannelGroup::Unknown,
            const bool read_mode = true
        );
        ~WAVFile() override;

        const format_t&  getFormat() const noexcept;

        void save_prepare(const uint32_t freq, const bool stereo);
        void save_streaming(const int16_t* buffer, const size_t length);
        void save_end();
        void save(const uint32_t freq, const bool stereo, const int16_t* sound, const size_t length);
        void save(const audio::Sound& sound);


        ///static bool save(const int rate, const int bits, const int channels, const uint8_t* buffer, const int length);
        //static bool render(const uint8_t* buffer, int length);

    private:
        format_t m_fmt_chunk;

        bool m_expDataChunk = false;
        bool m_isSaving = false;
        const bool m_read_mode;
        const std::streampos m_saving_length_pos = sizeof(RIFF_chunk_header_t) - sizeof(uint32_t) - sizeof(RIFF_ID);
        const std::streampos m_saving_data_length_pos = 40;

        /// <summary>
        /// read the 'fmt ' sub-chunks, file need to be positioned
        /// just after the sub chunk header and pass it as
        /// parameter.
        /// </summary>
        bool read_fmt_sub_chunk(const RIFF_sub_chunk_header_t& chunk);
        bool read_data_sub_chunk(const RIFF_sub_chunk_header_t& chunk);
    };
}
