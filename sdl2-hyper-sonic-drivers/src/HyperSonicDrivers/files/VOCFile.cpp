#include <vector>
#include <cstring>
#include <format>
#include <HyperSonicDrivers/files/VOCFile.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>

namespace HyperSonicDrivers::files
{
    using utils::logW;

    constexpr const char* MAGIC = "Creative Voice File\x1A";
    constexpr const uint16_t VALIDATION_MAGIC = 0x1234;

    VOCFile::VOCFile(const std::string& filename, const audio::mixer::eChannelGroup group) :
        File(filename), IPCMFile(),
        m_version(0)
    {
        m_channels = 1;
        m_bitsDepth = 8;
        assertValid_(readHeader());
        assertValid_(readDataBlockHeader());

        make_pcm_sound_(group);
    }

    const std::string VOCFile::getVersion() const noexcept
    {
        return std::format("{}.{}", (m_version >> 8), (m_version & 0xFF));
    }

    bool VOCFile::readHeader()
    {
        bool res = false;
        voc_header_t header;

        read(&header, sizeof(voc_header_t));
        res = strncmp(header.magic, MAGIC, MAGIC_SIZE) == 0;
        res &= ((~header.version) + VALIDATION_MAGIC) == header.validation_code;
        res &= tell() == header.data_block_offset;
        m_version = header.version;

        return res;
    }

    bool VOCFile::readDataBlockHeader()
    {
        uint8_t lastType;
        std::vector<uint8_t> buf;

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
                assertValid_(db.data[0] == 0 && db.size == 1);
                break;
            case 1: // sound block
            {
                uint8_t timeConstant = db.data[0];
                uint8_t packMethod = db.data[1];
                // time constant
                // channels default = 1
                // timeConstant = 65536 - (256000000 / (channels * sampleRate);
                // sampleRate = 256000000 / ((65536 - (timeConstant<<8))*channels)
                m_sampleRate = 256000000L / ((65536 - (timeConstant << 8)) * m_channels);
                //_sampleRate = 1000000 / (256 - timeConstant);
                assertValid_(m_sampleRate == (1000000 / (256 - timeConstant)));
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
                    logW(std::format("unknown/not-implemented packMethod={:x}", packMethod));
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
                logW("pause block not-implemented");
            }
            break;
            case 4: // Marker block
                //uint16_t marker = readLE16();
                logW("marker block not-implemented");
                break;
            case 5: // null-terminating string block
                // TODO
                //std::string string = _readStringFromFile();
                //char* string = new char[data_block_size];
                //read(string, data_block_size);
                //delete string;
                logW("string block not-implemented");
                break;
            case 6: // loop block
                // TODO
                //uint16_t repeatTimes = readLE16();
                logW("start loop block not-implemented");
                break;
            case 7:
                logW("end loop block not-implemented");
                break;
            case 8:
                logW("special block 8 not-implemented");
                break;
            case 9:
            {
                assertValid_(m_version >= 0x0114);
                m_sampleRate = db.data[0] + (db.data[1] << 8) + (db.data[2] << 16) + (db.data[3] << 24);
                m_bitsDepth = db.data[4];
                m_channels = db.data[5];
                uint16_t format = db.data[6] + (db.data[7] << 8);
                for (int i = 0; i < 4; i++)
                    assertValid_(db.data[i + 8] == 0);
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
                    logW(std::format("unknown/not-implemented format={}", format));
                }
            }
                break;
            default:
                //return false;
                logW(std::format("unknown data block type {}", db.type));
            }

            lastType = db.type; // ?
        }

        int divisor = 1;
        if (m_bitsDepth == 16) {
            divisor *= 2;
        }
        if (m_channels == 2) {
            divisor *= 2;
        }

        const int d = buf.size() % divisor;
        for (int i = 0; i < d; i++)
            buf.push_back(0);

        m_dataSize = static_cast<uint32_t>(buf.size());
        m_data = std::make_shared<uint8_t[]>(m_dataSize);
        std::memcpy(m_data.get(), buf.data(), sizeof(uint8_t)* m_dataSize);

        return true;
    }

    VOCFile::sub_data_block_t VOCFile::readSubDataBlock(const uint32_t data_block_size, const uint8_t type) const noexcept
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
