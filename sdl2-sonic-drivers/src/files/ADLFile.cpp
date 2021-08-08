#include "ADLFile.hpp"

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

    constexpr int V1_HEADER_SIZE = 120 * sizeof(uint8_t);
    constexpr int V2_HEADER_SIZE = 120 * sizeof(uint8_t);
    constexpr int V3_HEADER_SIZE = 250 * sizeof(uint16_t);
    
    constexpr int V1_NUM_PROGRAM_OFFSETS = 150;
    constexpr int V2_NUM_PROGRAM_OFFSETS = 250;
    constexpr int V3_NUM_PROGRAM_OFFSETS = 500;

    constexpr int V1_NUM_INSTRUMENT_OFFSETS = 150;
    constexpr int V2_NUM_INSTRUMENT_OFFSETS = 250;
    constexpr int V3_NUM_INSTRUMENT_OFFSETS = 500;

    constexpr int V1_DATA_OFFSET = V1_NUM_PROGRAM_OFFSETS * sizeof(uint16_t) + V1_NUM_INSTRUMENT_OFFSETS * sizeof(uint16_t) + V1_HEADER_SIZE;
    constexpr int V2_DATA_OFFSET = V2_NUM_PROGRAM_OFFSETS * sizeof(uint16_t) + V2_NUM_INSTRUMENT_OFFSETS * sizeof(uint16_t) + V2_HEADER_SIZE;
    constexpr int V3_DATA_OFFSET = V3_NUM_PROGRAM_OFFSETS * sizeof(uint16_t) + V3_NUM_INSTRUMENT_OFFSETS * sizeof(uint16_t) + V3_HEADER_SIZE;

    constexpr int V1_OFFSET_START = V1_DATA_OFFSET - V1_HEADER_SIZE;
    constexpr int V2_OFFSET_START = V2_DATA_OFFSET - V2_HEADER_SIZE;
    constexpr int V3_OFFSET_START = V3_DATA_OFFSET - V3_HEADER_SIZE;

    constexpr int FILE_SIZE_MIN = V1_DATA_OFFSET;

    ADLFile::ADLFile(const std::string& filename) : File(filename)
    {
        _assertValid(size() >= FILE_SIZE_MIN);
        _detectVersion();
        _validateVersion();
    }

    uint8_t ADLFile::getVersion() const noexcept
    {
        return _version;
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
            for (int i = 0; i < V1_NUM_PROGRAM_OFFSETS; i++)
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
        uintmax_t min_size = 0;
        switch (_version)
        {
        case 1:
            min_size = V1_DATA_OFFSET;
            break;
        case 2:
            min_size = V2_DATA_OFFSET;
            break;
        case 3:
            min_size = V3_DATA_OFFSET;
            break;
        default:
            // unreachable code.
            throw std::invalid_argument("ADL File version not detected");
        }

        _assertValid(size() > min_size);
    }
}
