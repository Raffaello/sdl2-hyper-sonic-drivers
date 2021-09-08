#pragma once

#include <files/File.hpp>
#include <string>
#include <cstdint>

namespace files
{
    class VOCFile : public File
    {
    public:
        VOCFile(const std::string& filename);
        virtual ~VOCFile();

        const std::string getVersion() const noexcept;

    private:
        uint16_t _version;

        static const int MAGIC_SIZE = 19;
        typedef struct voc_header_t
        {
            char magic[MAGIC_SIZE];
            uint16_t data_block_offset;
            uint16_t version;
            uint16_t validation_code; // complement(version) + 0x1234
        } voc_header_t;

        typedef struct data_block_header_t
        {
            uint8_t type;
            uint8_t length[3]; // 24 bit length, convert after reading
        } data_block_header_t;

        bool readHeader();
        bool validate_header(const voc_header_t& header) noexcept;
        bool readDataBlockHeader();
    };
}
