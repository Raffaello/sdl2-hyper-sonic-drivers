#pragma once

#include <audio/Sound.hpp>
#include <audio/scummvm/Mixer.hpp>
#include <files/RIFFFile.hpp>
#include <string>
#include <memory>

namespace files
{
    class WAVFile final : public RIFFFile
    {
    public:
        enum class eFormat
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
        // BODY: can be splitted in format_common struct
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
        static_assert(2 + 2 + 4 + 4 + 2 + 2 == sizeof(format_t) - sizeof(eFormat));

        WAVFile(const std::string& filename, const audio::scummvm::Mixer::SoundType soundType = audio::scummvm::Mixer::SoundType::PLAIN);
        virtual ~WAVFile();

        const format_t&                  getFormat()   const noexcept;
        const uint32_t                   getDataSize() const noexcept;
        const std::shared_ptr<uint8_t[]> getData()     const noexcept;
        std::shared_ptr<audio::Sound>    getSound()    const noexcept;
        
        static bool save(const int rate, const int bits, const int channels, const uint8_t* buffer, const int length);
        static bool render(const uint8_t* buffer, int length);


    private:
        format_t _fmt_chunk;
        uint32_t _dataSize;
        std::shared_ptr<uint8_t[]> _data;
        std::shared_ptr<audio::Sound> _sound;

        bool _expDataChunk = false;
        
        /// <summary>
        /// read the 'fmt ' subchunks, file need to positioned
        /// just after the sub chunk header and pass it as
        /// parameter.
        /// </summary>
        bool read_fmt_sub_chunk(const RIFF_sub_chunk_header_t& chunk);
        bool read_data_sub_chunk(const RIFF_sub_chunk_header_t& chunk);
    };
}
