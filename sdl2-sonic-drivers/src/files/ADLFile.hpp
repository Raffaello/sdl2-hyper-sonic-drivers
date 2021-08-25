#pragma once

#include <files/File.hpp>
#include <string>
#include <vector>
#include <functional>
#include <limits>

namespace files
{
    class ADLFile final : public File
    {
    public:
        ADLFile(const std::string& filename);
        uint8_t getVersion() const noexcept;
        int getNumTracks() const noexcept;
        int getNumTrackOffsets() const noexcept;
        int getNumInstrumentOffsets() const noexcept;
        uint8_t getTrack(const int track) const;
        uint16_t getTrackOffset(const int programId) const;
        uint16_t getInstrumentOffset(const int instrument) const;
        uint32_t getDataSize() const noexcept;
        const std::vector<uint8_t>& getData() const noexcept;
        const int getNumPrograms() const noexcept;

    private:
        uint8_t _version = 0;
        void _detectVersion();
        void _validateVersion();
        
        void _readHeader();
        void _readHeaderFromFile(const int header_size, std::function<uint8_t()> read);
        
        void _readTrackOffsets();
        void _readInstrumentOffsets();
        void _readOffsetsFromFile(const int num_offsets, std::vector<uint16_t>& vec, const int offset_start);

        void _readData();
        void _readDataFromFile(const int data_offsets);
        
        std::vector<uint8_t> _header;
        std::vector<uint16_t> _track_offsets;
        std::vector<uint16_t> _instrument_offsets;
        std::vector<uint8_t> _data;
        
        void _count_tracks();
        void _count_track_offsets();
        void _count_instruments();
        template<typename T>
        int _count_loop(const int num_offs, const std::vector<T>& vec);

        int _num_tracks = -1;
        int _num_track_offsets = -1;
        int _num_instrument_offsets = -1;
        int _num_programs = -1;

        void _functor(std::function<void()> funcV1, std::function<void()> funcV2, std::function<void()> funcV3);
    };

    template<typename T>
    int ADLFile::_count_loop(const int offs_start, const std::vector<T>& vec)
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
