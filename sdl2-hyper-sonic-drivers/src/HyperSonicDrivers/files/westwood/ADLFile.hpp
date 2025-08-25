#pragma once

#include <string>
#include <vector>
#include <functional>
#include <limits>
#include <memory>
#include <HyperSonicDrivers/files/File.hpp>
#include <HyperSonicDrivers/hardware/opl/OPL2instrument.h>

namespace HyperSonicDrivers::files::westwood
{
    class ADLFile : protected File
    {
    public:
        enum class PROG_TYPE
        {
            Track,
            Instrument
        };

        typedef struct meta_version_t
        {
            uint8_t num_headers;
            uint16_t header_size;
            uint16_t data_offset;
            uint16_t num_track_offsets;
            uint16_t track_offsets_size;
            uint16_t num_instrument_offsets;
            uint16_t offset_start;
            uint16_t data_header_size;
        } meta_version_t;

        ADLFile(const std::string &filename);
        ~ADLFile() override = default;

        uint8_t getVersion() const noexcept;

        uint16_t getNumTracks() const noexcept;
        uint16_t getNumTrackOffsets() const noexcept;
        uint16_t getNumInstrumentOffsets() const noexcept;

        uint16_t getTrack(const uint16_t track) const;
        uint16_t getTrackOffset(const uint16_t programId) const;
        uint16_t getInstrumentOffset(const uint16_t instrument) const;
        uint16_t getProgramOffset(const uint16_t progId, const PROG_TYPE prog_type) const;

        uint32_t getDataSize() const noexcept;
        std::shared_ptr<uint8_t[]> getData() const noexcept;

        hardware::opl::OPL2instrument_t getInstrument(const int instrument) const;

    private:
        uint8_t m_version = 0;
        meta_version_t m_meta_version;
        std::function<uint16_t()> m_read;

        void detectVersion_();

        void readHeaderFromFile_(const uint16_t header_size, std::function<uint16_t()> read);
        void readOffsetsFromFile_(const uint16_t num_offsets, std::vector<uint16_t> &vec, const uint32_t offset_start) const;
        void readDataFromFile_(const int data_offsets, const int data_header_size);

        std::vector<uint16_t> m_header;
        std::vector<uint16_t> m_track_offsets;
        std::vector<uint16_t> m_instrument_offsets;
        std::shared_ptr<uint8_t[]> m_data;
        uint32_t m_dataSize = 0;
        int m_dataHeaderSize = 0;

        template <typename T>
        int count_loop_(const int num_offs, const std::vector<T> &vec);
        void adjust_offsets_(std::vector<uint16_t> &vec);
        uint16_t m_num_tracks = 0;
        uint16_t m_num_track_offsets = 0;
        uint16_t m_num_instrument_offsets = 0;
    };

    template <typename T>
    int ADLFile::count_loop_(const int offs_start, const std::vector<T> &vec)
    {
        int tot = 0;
        constexpr T max_ = std::numeric_limits<T>::max();
        for (size_t i = 0; i < vec.size(); ++i)
        {
            if (vec[i] >= offs_start && vec[i] < max_)
            {
                ++tot;
            }
        }

        return tot;
    }
}
