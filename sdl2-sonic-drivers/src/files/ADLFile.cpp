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

    constexpr int V1_NUM_INSTRUMENT_OFFSETS = 150;
    constexpr int V2_NUM_INSTRUMENT_OFFSETS = 250;
    constexpr int V3_NUM_INSTRUMENT_OFFSETS = 500;

    constexpr int V1_DATA_OFFSET = V1_NUM_TRACK_OFFSETS * sizeof(uint16_t) + V1_NUM_INSTRUMENT_OFFSETS * sizeof(uint16_t) + V1_HEADER_SIZE;
    constexpr int V2_DATA_OFFSET = V2_NUM_TRACK_OFFSETS * sizeof(uint16_t) + V2_NUM_INSTRUMENT_OFFSETS * sizeof(uint16_t) + V2_HEADER_SIZE;
    constexpr int V3_DATA_OFFSET = V3_NUM_TRACK_OFFSETS * sizeof(uint16_t) + V3_NUM_INSTRUMENT_OFFSETS * sizeof(uint16_t) + V3_HEADER_SIZE;

    constexpr int V1_OFFSET_START = V1_DATA_OFFSET - V1_HEADER_SIZE;
    constexpr int V2_OFFSET_START = V2_DATA_OFFSET - V2_HEADER_SIZE;
    constexpr int V3_OFFSET_START = V3_DATA_OFFSET - V3_HEADER_SIZE;

    constexpr int FILE_SIZE_MIN = V1_DATA_OFFSET;

    ADLFile::ADLFile(const std::string& filename) : File(filename)
    {
        _assertValid(size() >= FILE_SIZE_MIN);
        _detectVersion();
        _validateVersion();

        _readHeader();
        _readTrackOffsets();
        _readInstrumentOffsets();

        _count_tracks();
        _count_instruments();

        // TODO: data section ???
    }

    uint8_t ADLFile::getVersion() const noexcept
    {
        return _version;
    }

    int ADLFile::getNumTracks() const noexcept
    {
        return _num_tracks;
    }

    int ADLFile::getNumInstruments() const noexcept
    {
        return _num_instruments;
    }

    void ADLFile::_detectVersion()
    {
        seek(0, std::fstream::_Seekbeg);
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
            seek(V1_HEADER_SIZE, std::fstream::_Seekbeg);
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

    void ADLFile::_validateVersion()
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

    void ADLFile::_readHeader()
    {
        seek(0, std::fstream::_Seekbeg);
        _functor(
            [this]() { _readHeaderFromFile(V1_HEADER_SIZE, std::bind(&ADLFile::readU8, this)); },
            [this]() { _readHeaderFromFile(V2_HEADER_SIZE, std::bind(&ADLFile::readU8, this)); },
            [this]() { _readHeaderFromFile(V3_HEADER_SIZE, std::bind(&ADLFile::readLE16, this)); }
        );
    }

    void ADLFile::_readHeaderFromFile(const int header_size, std::function<uint16_t()> read)
    {
        _header.resize(header_size);
        for (int i = 0; i < header_size; i++)
        {
            _header[i] = read();
            //_assertValid(_header[i] < offset_start);
        }
        _assertValid(_header.size() == header_size);
    }

    void ADLFile::_readTrackOffsets()
    {
        _functor(
            [this]() { _readOffsetsFromFile(V1_NUM_TRACK_OFFSETS, _track_offsets); },
            [this]() { _readOffsetsFromFile(V2_NUM_TRACK_OFFSETS, _track_offsets); },
            [this]() { _readOffsetsFromFile(V3_NUM_TRACK_OFFSETS, _track_offsets); }
        );
    }

    void ADLFile::_readInstrumentOffsets()
    {
        _functor(
            [this]() { _readOffsetsFromFile(V1_NUM_INSTRUMENT_OFFSETS, _instrument_offsets); },
            [this]() { _readOffsetsFromFile(V2_NUM_INSTRUMENT_OFFSETS, _instrument_offsets); },
            [this]() { _readOffsetsFromFile(V3_NUM_INSTRUMENT_OFFSETS, _instrument_offsets); }
        );
    }

    void ADLFile::_readOffsetsFromFile(const int num_offsets, std::vector<uint16_t>& vec)
    {
        vec.resize(num_offsets);
        for (int i = 0; i < num_offsets; i++) {
            vec[i] = readLE16();
        }
        _assertValid(vec.size() == num_offsets);
    }

    void ADLFile::_count_tracks()
    {
        _functor(
            [this]() { _num_tracks = _count_loop(V1_NUM_TRACK_OFFSETS, V1_OFFSET_START, _track_offsets); },
            [this]() { _num_tracks = _count_loop(V2_NUM_TRACK_OFFSETS, V2_OFFSET_START, _track_offsets); },
            [this]() { _num_tracks = _count_loop(V3_NUM_TRACK_OFFSETS, V3_OFFSET_START, _track_offsets); }
        );
    }

    void ADLFile::_count_instruments()
    {
        _functor(
            [this]() { _num_instruments = _count_loop(V1_NUM_INSTRUMENT_OFFSETS, V1_OFFSET_START, _instrument_offsets); },
            [this]() { _num_instruments = _count_loop(V2_NUM_INSTRUMENT_OFFSETS, V2_OFFSET_START, _instrument_offsets); },
            [this]() { _num_instruments = _count_loop(V3_NUM_INSTRUMENT_OFFSETS, V3_OFFSET_START, _instrument_offsets); }
        );
    }

    int ADLFile::_count_loop(const int num_header, const int offs_start, const std::vector<uint16_t>& vec)
    {
        int tot = 0;
        for (int i = 0; i < num_header; ++i)
        {
            if (vec[i] >= offs_start && vec[i] < 0xFFFF) {
                ++tot;
            }
        }

        return tot;
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
