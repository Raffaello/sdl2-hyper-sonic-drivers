#include <files/VOCFile.hpp>
#include <cstring>
#include <vector>
#include <spdlog/spdlog.h>

namespace files
{
    constexpr const char* MAGIC = "Creative Voice File\x1A";
    constexpr const uint16_t VALIDATION_MAGIC = 0x1234;

    VOCFile::VOCFile(const std::string& filename, const audio::scummvm::Mixer::SoundType soundType) : File(filename),
        _version(0), _channels(1), _bitsDepth(8)
    {
        _assertValid(readHeader());
        _assertValid(readDataBlockHeader());

        _sound = std::make_shared<audio::Sound>(
            soundType,
            getChannels() == 2,
            getSampleRate(),
            getBitsDepth(),
            getDataSize(),
            getData()
        );
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

    const int VOCFile::getBitsDepth() const noexcept
    {
        return _bitsDepth;
    }

    const int VOCFile::getDataSize() const noexcept
    {
        return _dataSize;
    }

    const std::shared_ptr<uint8_t[]> VOCFile::getData() const noexcept
    {
        return _data;
    }

    std::shared_ptr<audio::Sound> VOCFile::getSound() const noexcept
    {
        return _sound;
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
        std::vector<uint8_t> buf;
        uint8_t lastType;
 
        while (true)
        {
            uint8_t type = readU8();
            if (type == 0)
                break;

            uint32_t a = readU8();
            uint32_t b = readU8() << 8;
            uint32_t c = readU8() << 16;
            uint32_t data_block_size = a + b + c;

            sub_data_block_t db = readSubDataBlock(data_block_size, type);

            // TODO add streaming capability from the file later on
            switch (db.type)
            {
            case 0: // End of Data block
                _assertValid(db.data[0] == 0 && db.size == 1);
                break;
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
                    spdlog::warn("VOCFile: unknown/not-implemented packMethod={}", packMethod);
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
                spdlog::warn("VOCFile: pause block not-implemented");
            }
            break;
            case 4: // Marker block
                //uint16_t marker = readLE16();
                spdlog::warn("VOCFile: marker block not-implemented");
                break;
            case 5: // null-terminating string block
                // TODO
                //std::string string = _readStringFromFile();
                //char* string = new char[data_block_size];
                //read(string, data_block_size);
                //delete string;
                spdlog::warn("VOCFile: string block not-implemented");
                break;
            case 6: // loop block
                // TODO
                //uint16_t repeatTimes = readLE16();
                spdlog::warn("VOCFile: start loop block not-implemented");
                break;
            case 7:
                spdlog::warn("VOCFile: end loop block not-implemented");
                break;
            case 8:
                spdlog::warn("VOCFile: special block 8 not-implemented");
                break;
            case 9:
            {
                _assertValid(_version >= 0x0114);
                _sampleRate = db.data[0] + (db.data[1] << 8) + (db.data[2] << 16) + (db.data[3] << 24);
                _bitsDepth = db.data[4];
                _channels = db.data[5];
                uint16_t format = db.data[6] + (db.data[7] << 8);
                for (int i = 0; i < 4; i++)
                    _assertValid(db.data[i + 8] == 0);
                // TODO is a super set of case 1, 4 first cases.
                // BODY: create a function to process format.
                switch (format)
                {
                case 0: // 8-bit unsigned PCM
                case 4: // 16-bit signed PCM
                    for (int i = 12; i < db.size; i++) {
                        buf.push_back(db.data[i]);
                    }
                    break;
                case 1: // 8-bit 4-bit ADPCM
                    //break;
                case 2: // 8-bit 3-bit ADPCM
                    //break;
                case 3:// 8-bit 2-bit ADPCM
                    //break;
                case 6: // CCITT a-Law
                    //break;
                case 7: // CCITT mu-Law
                    //break;
                case 0x0200: // 16-bit to 4-bit ADPCM
                    //break;
                default:
                    spdlog::warn("VOCFile: unknown/not-implemented format={}", format);
                }
            }
                break;
            default:
                //return false;
                spdlog::warn("VOCFile: unknown data block type {}", db.type);
            }

            lastType = db.type; // ?
        }

        // copy vector buf to _data
        _dataSize = buf.size();
        // sanity check
        int divisor = 1;
        if (_bitsDepth == 16) {
            divisor *= 2;
        }
        if (_channels == 2) {
            divisor *= 2;
        }

        //_assertValid(_dataSize % divisor == 0);
        _dataSize = buf.size() + (buf.size() % divisor);
        uint8_t* b = new uint8_t[_dataSize];
        for (int i = 0; i < buf.size(); i++) {
            b[i] = buf[i];
        }

        for (int i = buf.size(); i < _dataSize; i++) {
            b[i] = 0;
        }

        _data.reset(b);
        buf.clear();

        return true;
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
}
