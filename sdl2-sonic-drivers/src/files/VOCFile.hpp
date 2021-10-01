#pragma once

#include <audio/scummvm/Mixer.hpp>
#include <audio/Sound.hpp>
#include <files/File.hpp>
#include <string>
#include <cstdint>
#include <memory>

namespace files
{
    // TODO create&implement a PCMFile interface
    class VOCFile final : protected File
    {
    public:
        VOCFile(const std::string& filename, const audio::scummvm::Mixer::SoundType soundType = audio::scummvm::Mixer::SoundType::PLAIN);
        virtual ~VOCFile();

        const std::string getVersion() const noexcept;
        const int getChannels() const noexcept;
        const int getSampleRate() const noexcept;
        const int getBitsDepth() const noexcept;
        const int getDataSize() const noexcept;
        const std::shared_ptr<uint8_t[]> getData() const noexcept;

        std::shared_ptr<audio::Sound> getSound() const noexcept;

    private:
        static const int MAGIC_SIZE = 19 + 1;
        typedef struct voc_header_t
        {
            char magic[MAGIC_SIZE];
            uint16_t data_block_offset; // 0x1A
            uint16_t version;
            uint16_t validation_code;   // complement(version) + 0x1234
        } voc_header_t;
        static_assert(sizeof(voc_header_t) == MAGIC_SIZE + 2 + 2 + 2);

        typedef struct sub_data_block_t
        {
            uint8_t type;
            uint32_t size;
            std::shared_ptr<uint8_t[]> data;
        } sub_data_block_t;

        uint16_t _version;
        // VOC to PCM info
        int       _channels;
        uint32_t  _sampleRate;
        int       _dataSize;
        uint8_t   _bitsDepth;
        std::shared_ptr<uint8_t[]> _data;
        std::shared_ptr<audio::Sound> _sound;

        bool readHeader();
        bool readDataBlockHeader();
        sub_data_block_t readSubDataBlock(const uint32_t data_block_size, const uint8_t type);
    };
}
