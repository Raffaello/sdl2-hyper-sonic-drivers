#pragma once

#include <files/File.hpp>
#include <string>
#include <vector>
#include <functional>
#include <limits>
#include <memory>


namespace files
{
    class ADLFile final : public File
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
        //const int getNumPrograms() const noexcept;
        //uint16_t getNumTrackOffset(const int progId) const noexcept;

    private:
        uint8_t _version = 0;
        void detectVersion();
        void validateVersion();
        
        void readHeader();
        void readHeaderFromFile(const int header_size, std::function<uint8_t()> read);
        
        void readTrackOffsets();
        void readInstrumentOffsets();
        void readOffsetsFromFile(const int num_offsets, std::vector<uint16_t>& vec, const int offset_start);

        void readData();
        void readDataFromFile(const int data_offsets, const int data_heder_size);
        
        std::vector<uint8_t> _header;
        std::vector<uint16_t> _track_offsets;
        std::vector<uint16_t> _instrument_offsets;
        std::shared_ptr<uint8_t[]> _data;
        uint32_t _dataSize = 0;
        int _dataHeaderSize = 0;
        
        void count_tracks();
        void count_track_offsets();
        void count_instruments();
        template<typename T>
        int count_loop(const int num_offs, const std::vector<T>& vec);

        int _num_tracks = -1;
        int _num_track_offsets = -1;
        int _num_instrument_offsets = -1;
        //int _num_programs = -1;

        void _functor(std::function<void()> funcV1, std::function<void()> funcV2, std::function<void()> funcV3);
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
