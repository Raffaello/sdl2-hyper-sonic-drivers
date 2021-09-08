#pragma once

#include <files/RIFFFile.hpp>
#include <string>
#include <memory>

namespace files
{
    class WAVFile : public RIFFFile
    {
    public:
        enum class eFormat
        {
            WAVE_FORMAT_PCM  = 0x0001,
            IBM_FORMAT_MULAW = 0x0101,
            IBM_FORMAT_ALAW  = 0x0102,
            IBM_FORMAT_ADPCM = 0x0103,
            WAVE_FORMAT_IEEE_FLOAT = 0x0003,
            WAVE_FORMAT_ALAW = 0x0006,
            WAVE_FORMAT_MULAW = 0x007,
            WAVE_FORMAT_EXTENSIBLE = 0xFFFE
        };

        WAVFile(const std::string& filename);
        virtual ~WAVFile();
        static bool save(const int rate, const int bits, const int channels, const uint8_t* buffer, const int length);
        static bool render(const uint8_t* buffer, int length);

    private:
        eFormat  _format;
        uint16_t _channels;
        uint32_t _samplesPerSec;
        uint32_t _avgBytesPerSec;
        uint16_t _blockAlign;
        uint16_t _bitsPerSample; // format 1 (PCM)

        uint32_t _dataSize;
        std::shared_ptr<uint8_t[]> _data;
    };
}
