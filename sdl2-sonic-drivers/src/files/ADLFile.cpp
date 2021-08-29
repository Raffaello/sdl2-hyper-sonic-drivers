#include "ADLFile.hpp"
#include <exception>
#include <fstream>

namespace files
{
    /***************************************************************************
     * Here's the rough structure of ADL files. The player needs only the      *
     * first part, the driver uses only the second part (all offsets in the    *
     * programs/instruments tables are relative to the start of _soundData).   *
     *                                                                         *
     * _trackEntries[]      _soundData[]                                       *
     * +------------------+ +-----------------+--------------------+------//-+ *
     * | subsong->prog.ID | | Program offsets | Instrument offsets | Data... | *
     * +------------------+ +-----------------+--------------------+------//-+ *
     * v1:   120 bytes           150 words          150 words      @720        *
     * v2:   120 bytes           250 words          250 words      @1120       *
     * v3:   250 words           500 words          500 words      @2500       *
     *                                                                         *
     * The versions can be distinguished by inspecting the table entries.      *
     ***************************************************************************/

    constexpr int V1_NUM_HEADER = 120;
    constexpr int V2_NUM_HEADER = 120;
    constexpr int V3_NUM_HEADER = 250;
    
    constexpr int V1_HEADER_SIZE = V1_NUM_HEADER * sizeof(uint8_t);
    constexpr int V2_HEADER_SIZE = V2_NUM_HEADER * sizeof(uint8_t);
    constexpr int V3_HEADER_SIZE = V3_NUM_HEADER * sizeof(uint16_t);
    
    constexpr int V1_NUM_TRACK_OFFSETS = 150;
    constexpr int V2_NUM_TRACK_OFFSETS = 250;
    constexpr int V3_NUM_TRACK_OFFSETS = 500;

    constexpr int V1_TRACK_OFFSETS_SIZE = V1_NUM_TRACK_OFFSETS * sizeof(uint16_t);
    constexpr int V2_TRACK_OFFSETS_SIZE = V2_NUM_TRACK_OFFSETS * sizeof(uint16_t);
    constexpr int V3_TRACK_OFFSETS_SIZE = V3_NUM_TRACK_OFFSETS * sizeof(uint16_t);

    constexpr int V1_NUM_INSTRUMENT_OFFSETS = 150;
    constexpr int V2_NUM_INSTRUMENT_OFFSETS = 250;
    constexpr int V3_NUM_INSTRUMENT_OFFSETS = 500;

    constexpr int V1_INSTRUMENT_OFFSETS_SIZE = V1_NUM_INSTRUMENT_OFFSETS * sizeof(uint16_t);
    constexpr int V2_INSTRUMENT_OFFSETS_SIZE = V2_NUM_INSTRUMENT_OFFSETS * sizeof(uint16_t);
    constexpr int V3_INSTRUMENT_OFFSETS_SIZE = V3_NUM_INSTRUMENT_OFFSETS * sizeof(uint16_t);

    constexpr int V1_DATA_HEADER_SIZE = V1_TRACK_OFFSETS_SIZE + V1_INSTRUMENT_OFFSETS_SIZE;
    constexpr int V2_DATA_HEADER_SIZE = V2_TRACK_OFFSETS_SIZE + V2_INSTRUMENT_OFFSETS_SIZE;
    constexpr int V3_DATA_HEADER_SIZE = V3_TRACK_OFFSETS_SIZE + V3_INSTRUMENT_OFFSETS_SIZE;

    constexpr int V1_DATA_OFFSET = V1_DATA_HEADER_SIZE + V1_HEADER_SIZE;
    constexpr int V2_DATA_OFFSET = V2_DATA_HEADER_SIZE + V2_HEADER_SIZE;
    constexpr int V3_DATA_OFFSET = V3_DATA_HEADER_SIZE + V3_HEADER_SIZE;

    constexpr int V1_OFFSET_START = V1_DATA_OFFSET - V1_HEADER_SIZE;
    constexpr int V2_OFFSET_START = V2_DATA_OFFSET - V2_HEADER_SIZE;
    constexpr int V3_OFFSET_START = V3_DATA_OFFSET - V3_HEADER_SIZE;

    constexpr int FILE_SIZE_MIN = V1_DATA_OFFSET;

    ADLFile::ADLFile(const std::string& filename) : File(filename)
    {
        _assertValid(size() >= FILE_SIZE_MIN);
        detectVersion();
        validateVersion();

        readHeader();
        readTrackOffsets();
        readInstrumentOffsets();
        readData();

        count_tracks();
        count_track_offsets();
        count_instruments();
        
        // Closing file
        close();

        // Adjust Offsets
        adjust_offsets(_track_offsets);
        adjust_offsets(_instrument_offsets);
    }

    uint8_t ADLFile::getVersion() const noexcept
    {
        return _version;
    }

    int ADLFile::getNumTracks() const noexcept
    {
        return _num_tracks;
    }

    int ADLFile::getNumTrackOffsets() const noexcept
    {
        return _num_track_offsets;
    }

    int ADLFile::getNumInstrumentOffsets() const noexcept
    {
        return _num_instrument_offsets;
    }

    uint8_t ADLFile::getTrack(const int track) const
    {
        return _header.at(track);
    }

    uint16_t ADLFile::getTrackOffset(const int programId) const
    {
        return _track_offsets.at(programId);
    }

    uint16_t ADLFile::getInstrumentOffset(const int instrument) const
    {
        return _instrument_offsets.at(instrument);
    }

    uint16_t ADLFile::getProgramOffset(const int progId, const PROG_TYPE prog_type) const
    {
        switch (prog_type)
        {
        case PROG_TYPE::TRACK:
            return getTrackOffset(progId);
        case PROG_TYPE::INSTRUMENT:
            return getInstrumentOffset(progId);
        }
    }

    uint32_t ADLFile::getDataSize() const noexcept
    {
        return _dataSize;
    }

    std::shared_ptr<uint8_t[]> ADLFile::getData() const noexcept
    {
        return _data;
    }

    void ADLFile::detectVersion()
    {
        seek(0, std::fstream::beg);
        // detect version 3
        _version = 3;
        for (int i = 0; i < V1_HEADER_SIZE; i++)
        {
            uint16_t v = readLE16();
            // For v3 all entries should be below 500 or equal 0xFFFF.
            // For other versions
            // this will check program offsets (at least 600).
            if ((v >= V3_HEADER_SIZE) && (v < 0xFFFF))
            {
                //v1,2
                _version = 2;
                break;
            }
        }

        // detect version 1,2
        if (_version < 3)
        {
            seek(V1_HEADER_SIZE, std::fstream::beg);
            // validate v1
            uint16_t min_w = 0xFFFF;
            for (int i = 0; i < V1_NUM_TRACK_OFFSETS; i++)
            {
                uint16_t w = readLE16();
                _assertValid(w >= V1_OFFSET_START || w == 0);
                if (min_w > w && w > 0) {
                    min_w = w;
                }
            }
            // detect
            if (min_w < V2_OFFSET_START)
            {
                _version = 1;
                _assertValid(min_w == V1_OFFSET_START);
            }
            else
            {
                _assertValid(min_w == V2_OFFSET_START);
            }
            
        }
    }

    void ADLFile::validateVersion()
    {
        // validate version with file size
        uintmax_t min_size = 0xFFFFF;
        _functor(
            [&min_size]() { min_size = V1_DATA_OFFSET; },
            [&min_size]() { min_size = V2_DATA_OFFSET; },
            [&min_size]() { min_size = V3_DATA_OFFSET; }
        );

        _assertValid(size() > min_size);
    }

    void ADLFile::readHeader()
    {
        seek(0, std::fstream::beg);
        _functor(
            [this]() { readHeaderFromFile(V1_NUM_HEADER, std::bind(&ADLFile::readU8, this)); },
            [this]() { readHeaderFromFile(V2_NUM_HEADER, std::bind(&ADLFile::readU8, this)); },
            [this]() { readHeaderFromFile(V3_NUM_HEADER, std::bind(&ADLFile::readLE16, this)); }
        );
    }

    void ADLFile::readHeaderFromFile(const int header_size, std::function<uint8_t()> read)
    {
        _header.resize(header_size);
        for (int i = 0; i < header_size; i++)
        {
            _header[i] = read();
        }

        _assertValid(_header.size() == header_size);
    }

    void ADLFile::readTrackOffsets()
    {
        _functor(
            [this]() { readOffsetsFromFile(V1_NUM_TRACK_OFFSETS, _track_offsets, V1_HEADER_SIZE); },
            [this]() { readOffsetsFromFile(V2_NUM_TRACK_OFFSETS, _track_offsets, V2_HEADER_SIZE); },
            [this]() { readOffsetsFromFile(V3_NUM_TRACK_OFFSETS, _track_offsets, V3_HEADER_SIZE); }
        );
    }

    void ADLFile::readInstrumentOffsets()
    {
        _functor(
            [this]() { readOffsetsFromFile(V1_NUM_INSTRUMENT_OFFSETS, _instrument_offsets, V1_TRACK_OFFSETS_SIZE + V1_HEADER_SIZE); },
            [this]() { readOffsetsFromFile(V2_NUM_INSTRUMENT_OFFSETS, _instrument_offsets, V2_TRACK_OFFSETS_SIZE + V2_HEADER_SIZE); },
            [this]() { readOffsetsFromFile(V3_NUM_INSTRUMENT_OFFSETS, _instrument_offsets, V3_TRACK_OFFSETS_SIZE + V3_HEADER_SIZE); }
        );
    }

    void ADLFile::readOffsetsFromFile(const int num_offsets, std::vector<uint16_t>& vec, const int offset_start)
    {
        _assertValid(tell() == offset_start);
        vec.resize(num_offsets);
        for (int i = 0; i < num_offsets; i++) {
            vec[i] = readLE16();
        }
        _assertValid(vec.size() == num_offsets);
    }

    void ADLFile::readData()
    {
        _functor(
            [this]() { readDataFromFile(V1_DATA_OFFSET, V1_DATA_HEADER_SIZE); },
            [this]() { readDataFromFile(V2_DATA_OFFSET, V2_DATA_HEADER_SIZE); },
            [this]() { readDataFromFile(V3_DATA_OFFSET, V3_DATA_HEADER_SIZE); }
        );
    }

    void ADLFile::readDataFromFile(const int data_offsets, const int data_heder_size)
    {
        _dataSize = size() - data_offsets;
        _assertValid(_dataSize > 0);
        _assertValid(tell() == data_offsets);
        _data.reset(new uint8_t[_dataSize]);
        read(_data.get(), _dataSize);
        _dataHeaderSize = data_heder_size;
    }

    void ADLFile::adjust_offsets(std::vector<uint16_t>& vec)
    {
        for (auto& v : vec)
        {
            if (v == 0)
                v = 0xFFFF;

            if (v == 0xFFFF)
                continue;

            _assertValid(v >= _dataHeaderSize);
            v -= _dataHeaderSize;
        }
    }

    void ADLFile::count_tracks()
    {
        _functor(
            [this]() { _num_tracks = count_loop<uint8_t>(0, _header); },
            [this]() { _num_tracks = count_loop<uint8_t>(0, _header); },
            [this]() { _num_tracks = count_loop<uint8_t>(0, _header); }
        );
    }

    void ADLFile::count_track_offsets()
    {
        _functor(
            [this]() { _num_track_offsets = count_loop<uint16_t>(V1_OFFSET_START, _track_offsets); },
            [this]() { _num_track_offsets = count_loop<uint16_t>(V2_OFFSET_START, _track_offsets); },
            [this]() { _num_track_offsets = count_loop<uint16_t>(V3_OFFSET_START, _track_offsets); }
        );
    }

    void ADLFile::count_instruments()
    {
        _functor(
            [this]() { _num_instrument_offsets = count_loop<uint16_t>(V1_OFFSET_START, _instrument_offsets); },
            [this]() { _num_instrument_offsets = count_loop<uint16_t>(V2_OFFSET_START, _instrument_offsets); },
            [this]() { _num_instrument_offsets = count_loop<uint16_t>(V3_OFFSET_START, _instrument_offsets); }
        );
    }

    void ADLFile::_functor(std::function<void()> funcV1, std::function<void()> funcV2, std::function<void()> funcV3)
    {
        switch (_version)
        {
        case 1:
            funcV1();
            break;
        case 2:
            funcV2();
            break;
        case 3:
            funcV3();
            break;
        default:
            throw std::invalid_argument("ADLFile: unrecognized version " + _version);
        }
    }
}
