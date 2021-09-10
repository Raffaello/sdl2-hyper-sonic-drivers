#include <files/VOCFile.hpp>
#include <cstring>
#include <memory>
#include <spdlog/spdlog.h>

namespace files
{
    constexpr const char* MAGIC = "Creative Voice File\x1A";
    constexpr const uint16_t VALIDATION_MAGIC = 0x1234;

    VOCFile::VOCFile(const std::string& filename) : File(filename),
        _version(0), _channels(1)
    {
        _assertValid(readHeader());
        _assertValid(readDataBlockHeader());
        
        processDataBlock();
    }

    VOCFile::~VOCFile()
    {}

    const std::string VOCFile::getVersion() const noexcept
    {
        return std::to_string(_version >> 8) + '.' + std::to_string(_version & 0xFF);
    }

    const int VOCFile::getChannels() const noexcept
    {
        return _channels;
    }

    const int VOCFile::getSampleRate() const noexcept
    {
        return _sampleRate;
    }

    const int VOCFile::getDataSize() const noexcept
    {
        return _dataSize;
    }

    const std::shared_ptr<uint8_t[]> VOCFile::getData() const noexcept
    {
        return _data;
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
        }

        return false;
    }

    VOCFile::sub_data_block_t VOCFile::readSubDataBlock(const uint32_t data_block_size, const uint8_t type)
    {
        std::shared_ptr<uint8_t[]> buf(new uint8_t[data_block_size]);
        read(buf.get(), data_block_size);
        sub_data_block_t db;
        db.type = type;
        db.size = data_block_size;
        db.data = buf;

        return db;
    }

    void VOCFile::processDataBlock()
    {
        std::vector<uint8_t> buf;
        uint8_t lastType;

        for (auto& db : _data_blocks)
        {
            switch (db.type)
            {
            case 0: // End of Data block
                _assertValid(db.data[0] == 0 && db.size == 1);
            case 1: // sound block
            {
                uint8_t timeConstant = db.data[0];
                uint8_t packMethod = db.data[1];
                // time constant
                // channels default = 1
                // timeConstant = 65536 - (256000000 / (channels * sampleRate);
                // sampleRate = 256000000 / ((65536 - (timeConstant<<8))*channels)
                _sampleRate = 256000000L / ((65536 - (timeConstant << 8)) * _channels);
                //_sampleRate = 1000000 / (256 - timeConstant);
                _assertValid(_sampleRate == (1000000 / (256 - timeConstant)));
                // pack Method
                switch (packMethod)
                {
                case 0: // 8-bit PCM
                    for (int i = 2; i < db.size; i++) {
                        buf.push_back(db.data[i]);
                    }
                    break;
                case 1: // 8-bit 4-bit ADPCM
                    //break;
                case 2: // 8-bit 3-bit ADPCM
                    //break;
                case 3:// 8-bit 2-bit ADPCM
                    //break;
                default:
                    spdlog::warn("VOCFile: unknown packMethod={}", packMethod);
                }

                
            }
            break;
            case 2: // continue sound block
            {
                for (int i = 0; i < db.size; i++) {
                    buf.push_back(db.data[i]);
                }
            }
            break;
            case 3: // pause block
            {
                // TODO
                //uint16_t pausePeriod = readLE16(); // pause in sample + 1
                //uint8_t  timeConstant = readU8(); // same as block 1
            }
            break;
            case 4: // Marker block
                //uint16_t marker = readLE16();
                break;
            case 5: // null-terminating string block
                // TODO
                //std::string string = _readStringFromFile();
                //char* string = new char[data_block_size];
                //read(string, data_block_size);
                //delete string;
                break;
            case 6: // loop block
                // TODO
                //uint16_t repeatTimes = readLE16();
                break;
            case 7:
                break;
            case 8:
                break;
            case 9:
                break;
            default:
                //return false;
                spdlog::warn("VOCFile: unknown data block type {}", db.type);
            }

            lastType = db.type;
        }

        // erase data_blocks
        _data_blocks.clear();

        // copy vector buf to _data
        _dataSize = buf.size();
        uint8_t* b = new uint8_t[_dataSize];
        for (int i = 0; i < _dataSize; i++) {
            b[i] = buf[i];
        }
        _data.reset(b);
    }
}
