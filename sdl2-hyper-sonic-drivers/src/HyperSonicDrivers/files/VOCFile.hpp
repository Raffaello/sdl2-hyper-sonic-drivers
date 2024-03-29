#pragma once

#include <HyperSonicDrivers/audio/mixer/ChannelGroup.hpp>
#include <HyperSonicDrivers/audio/PCMSound.hpp>
#include <HyperSonicDrivers/files/File.hpp>
#include <HyperSonicDrivers/files/IPCMFile.hpp>
#include <string>
#include <cstdint>
#include <memory>
#include <vector>

namespace HyperSonicDrivers::files
{
    class VOCFile final : protected File, public IPCMFile
    {
    public:
        VOCFile(const std::string& filename, const audio::mixer::eChannelGroup group = audio::mixer::eChannelGroup::Unknown);
        ~VOCFile() override = default;

        const std::string getVersion() const noexcept;

    private:
        static const int MAGIC_SIZE = 19 + 1;
        typedef struct voc_header_t
        {
            char magic[MAGIC_SIZE];
            uint16_t data_block_offset = 0; // 0x1A
            uint16_t version = 0;
            uint16_t validation_code = 0;   // complement(version) + 0x1234
        } voc_header_t;
        static_assert(sizeof(voc_header_t) == MAGIC_SIZE + 2 + 2 + 2);

        typedef struct sub_data_block_t
        {
            uint8_t type = 0;
            uint32_t size = 0;
            std::shared_ptr<uint8_t[]> data;
        } sub_data_block_t;

        uint16_t m_version;

        bool readHeader();
        bool readDataBlockHeader();
        sub_data_block_t readSubDataBlock(const uint32_t data_block_size, const uint8_t type) const noexcept;
    };
}
