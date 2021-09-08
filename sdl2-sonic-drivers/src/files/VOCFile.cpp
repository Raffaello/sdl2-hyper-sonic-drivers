#include <files/VOCFile.hpp>
#include <cstring>

namespace files
{
    constexpr const char* MAGIC = "Creative Voice File\x1A";
    constexpr const uint16_t VALIDATION_MAGIC = 0x1234;

    VOCFile::VOCFile(const std::string& filename) : File(filename),
        _version(0)
    {
        _assertValid(readHeader());
    }

    VOCFile::~VOCFile()
    {}

    const std::string VOCFile::getVersion() const noexcept
    {
        return std::to_string(_version >> 8) + '.' + std::to_string(_version & 0xFF);
    }

    bool VOCFile::readHeader()
    {
        bool res = false;
        voc_header_t header;

        read(&header, sizeof(voc_header_t));
        res = validate_header(header);
        _version = header.version;

        return res;
    }

    bool VOCFile::validate_header(const voc_header_t& header) noexcept
    {
        bool res = strncmp(header.magic, MAGIC, MAGIC_SIZE) == 0;
        res &=  ((~header.version) + VALIDATION_MAGIC) == header.validation_code;
        res &= tell() == header.data_block_offset;

        return res;
    }

    bool VOCFile::readDataBlockHeader()
    {
        while (!eof())
        {
            data_block_header_t data_header;
            read(&data_header, sizeof(data_block_header_t));
            uint32_t data_block_size = data_header.length[0] + data_header.length[1] << 8 + data_header.length[2] << 16;
            switch (data_header.type)
            {
            case 0: // End of Data block
                return  readU8() == 0 && data_block_size == 1;
            case 1:
                break;
            case 2:
                break;
            case 3:
                break;
            case 4:
                break;
            case 5:
                break;
            case 6:
                break;
            case 7:
                break;
            case 8:
                break;
            case 9:
                break;
            default:
                return false;
            }
        }
    }
}
