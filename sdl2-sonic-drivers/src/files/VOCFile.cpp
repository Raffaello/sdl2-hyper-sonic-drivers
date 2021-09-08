#include <files/VOCFile.hpp>
#include <cstring>
#include <memory>
#include <spdlog/spdlog.h>

namespace files
{
    constexpr const char* MAGIC = "Creative Voice File\x1A";
    constexpr const uint16_t VALIDATION_MAGIC = 0x1234;

    VOCFile::VOCFile(const std::string& filename) : File(filename),
        _version(0)
    {
        _assertValid(readHeader());
        _assertValid(readDataBlockHeader());
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
        res = strncmp(header.magic, MAGIC, MAGIC_SIZE) == 0;
        res &= ((~header.version) + VALIDATION_MAGIC) == header.validation_code;
        res &= tell() == header.data_block_offset;
        _version = header.version;

        return res;
    }


    bool VOCFile::readDataBlockHeader()
    {
        while (true)
        {
            uint8_t type = readU8();
            if (type == 0) {
                return true;
            }

            uint32_t data_block_size = readU8() + (readU8() << 8) + (readU8() << 16);
            sub_data_block_t db = readSubDataBlock(data_block_size, type);
            _data_blocks.push_back(db);

            /*
            switch (data_header.type)
            {
            case 0: // End of Data block
                return  readU8() == 0 && data_block_size == 1;
            case 1: // sound block
            {
                uint8_t timeConstant = readU8();
                uint8_t packMethod = readU8();
                data_block_size -= 2;
                sub_data_block_t db = readSubDataBlock(data_block_size, data_header.type);
                
                // time constant
                //auto res = 0xFFFF - (256000000 / (channels * sampleRate);

                // pack Method
                switch (packMethod)
                {
                case 0: // 8-bit PCM
                    break;
                case 1: // 8-bit 4-bit ADPCM
                    break;
                case 2: // 8-bit 3-bit ADPCM
                    break;
                case 3:// 8-bit 2-bit ADPCM
                    break;
                default:
                    spdlog::warn("VOCFile: unknown packMethod={}", packMethod);
                }

                _data_blocks.push_back(db);
            }
                break;
            case 2: // continue sound block
            {
                _data_blocks.push_back(readSubDataBlock(data_block_size, data_header.type));
            }
                break;
            case 3: // pause block
            {
                // TODO
                uint16_t pausePeriod = readLE16(); // pause in sample + 1
                uint8_t  timeConstant = readU8(); // same as block 1
            }
                break;
            case 4: // Marker block
                uint16_t marker = readLE16();
                break;
            case 5: // null-terminating string block
                // TODO
                //std::string string = _readStringFromFile();
                char* string = new char[data_block_size];
                read(string, data_block_size);
                delete string;
                break;
            case 6: // loop block
                // TODO
                uint16_t repeatTimes = readLE16();
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

            lastType = data_header.type;
            */
        }

        return false;
    }

    VOCFile::sub_data_block_t VOCFile::readSubDataBlock(const uint32_t data_block_size, const uint8_t type)
    {
        std::shared_ptr<uint8_t[]> buf(new uint8_t[data_block_size]);
        read(buf.get(), data_block_size);
        sub_data_block_t db;
        db.type = type;
        db.data = buf;

        return db;
    }
}
