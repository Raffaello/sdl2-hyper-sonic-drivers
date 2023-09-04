#pragma once

#include <HyperSonicDrivers/files/File.hpp>
#include <string>
#include <vector>
#include <functional>
#include <limits>
#include <memory>


namespace HyperSonicDrivers::files::westwood
{
    class ADLFile : protected File
    {
    public:
        enum class PROG_TYPE {
            TRACK, INSTRUMENT
        };
        ADLFile(const std::string& filename);
        uint8_t getVersion() const noexcept;
        int getNumTracks() const noexcept;
        int getNumTrackOffsets() const noexcept;
        int getNumInstrumentOffsets() const noexcept;
        uint8_t getTrack(const int track) const;
        uint16_t getTrackOffset(const int programId) const;
        uint16_t getInstrumentOffset(const int instrument) const;
        uint16_t getProgramOffset(const int progId, const PROG_TYPE prog_type) const;
        uint32_t getDataSize() const noexcept;
        std::shared_ptr<uint8_t[]> getData() const noexcept;

        typedef struct meta_version_t
        {
            uint8_t  num_headers;
            uint16_t header_size;
            uint16_t data_offset;
            uint16_t num_track_offsets;
            uint16_t track_offsets_size;
            uint16_t num_instrument_offsets;
            uint16_t offset_start;
            uint16_t data_header_size;
        } meta_version_t;
    private:
        uint8_t _version = 0;
        meta_version_t _meta_version;
        std::function<uint16_t()> _read;

        void detectVersion();

        void readHeaderFromFile(const int header_size, std::function<uint16_t()> read);
        void readOffsetsFromFile(const int num_offsets, std::vector<uint16_t>& vec, const int offset_start);
        void readDataFromFile(const int data_offsets, const int data_heder_size);

        std::vector<uint8_t> _header;
        std::vector<uint16_t> _track_offsets;
        std::vector<uint16_t> _instrument_offsets;
        std::shared_ptr<uint8_t[]> _data;
        uint32_t _dataSize = 0;
        int _dataHeaderSize = 0;

        template<typename T>
        int count_loop(const int num_offs, const std::vector<T>& vec);
        void adjust_offsets(std::vector<uint16_t>& vec);
        int _num_tracks = -1;
        int _num_track_offsets = -1;
        int _num_instrument_offsets = -1;
    };

    template<typename T>
    int ADLFile::count_loop(const int offs_start, const std::vector<T>& vec)
    {
        int tot = 0;
        constexpr T max_ = std::numeric_limits<T>::max();
        for (int i = 0; i < vec.size(); ++i)
        {
            if (vec[i] >= offs_start && vec[i] < max_) {
                ++tot;
            }
        }

        return tot;
    }
}

