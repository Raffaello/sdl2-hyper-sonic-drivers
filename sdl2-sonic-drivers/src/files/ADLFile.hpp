#pragma once

#include <files/File.hpp>
#include <string>
#include <vector>
#include <functional>
#include <fstream>

namespace files
{
    class ADLFile final : public File
    {
    public:
        ADLFile(const std::string& filename);
        uint8_t getVersion() const noexcept;
        int getNumTracks() const noexcept;
        int getNumInstruments() const noexcept;
        uint16_t getTrack(const int track) const;
        uint16_t getInstrument(const int instrument) const;
        uint32_t getDataSize() const noexcept;
        std::vector<uint8_t> getData() const noexcept;

    private:
        uint8_t _version = 0;
        void _detectVersion();
        void _validateVersion();
        
        void _readHeader();
        void _readHeaderFromFile(const int header_size, std::function<uint16_t()> read);
        
        void _readTrackOffsets();
        void _readInstrumentOffsets();
        void _readOffsetsFromFile(const int num_offsets, std::vector<uint16_t>& vec, const int offset_start);

        void _readData();
        void _readDataFromFile(const int data_offsets);
        
        std::vector<uint16_t> _header;
        std::vector<uint16_t> _track_offsets;
        std::vector<uint16_t> _instrument_offsets;
        std::vector<uint8_t> _data;
        
        void _count_tracks();
        void _count_instruments();
        int _count_loop(const int header_size, const int num_offs, const std::vector<uint16_t>& vec);

        int _num_tracks = -1;
        int _num_instruments = -1;

        void _functor(std::function<void()> funcV1, std::function<void()> funcV2, std::function<void()> funcV3);
    };
}
