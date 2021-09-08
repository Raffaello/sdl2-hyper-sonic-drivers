#pragma once

#include <files/File.hpp>
#include <string>
#include <cstdint>
#include <vector>
#include <memory>

namespace drivers {
    class VOCDriver;
}

namespace files
{
    class VOCFile : public File
    {
        friend drivers::VOCDriver;
    public:
        VOCFile(const std::string& filename);
        virtual ~VOCFile();

        const std::string getVersion() const noexcept;
        const int getChannels() const noexcept;
        const int getSampleRate() const noexcept;
        const int getDataSize() const noexcept;
        const std::shared_ptr<uint8_t[]> getData() const noexcept;

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
        std::vector<sub_data_block_t> _data_blocks;
        // VOC to PCM info
        int  _channels;
        int  _sampleRate;
        int  _dataSize;
        std::shared_ptr<uint8_t[]> _data;

        bool readHeader();
        bool readDataBlockHeader();
        sub_data_block_t readSubDataBlock(const uint32_t data_block_size, const uint8_t type);
        void processDataBlock();
        //void timeConstant();
    };
}
