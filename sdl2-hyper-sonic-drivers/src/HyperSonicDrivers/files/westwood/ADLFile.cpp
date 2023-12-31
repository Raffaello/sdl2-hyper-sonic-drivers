#include <HyperSonicDrivers/files/westwood/ADLFile.hpp>
#include <exception>
#include <fstream>

namespace HyperSonicDrivers::files::westwood
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

    const ADLFile::meta_version_t mv1 = {
        V1_NUM_HEADER,
        V1_HEADER_SIZE,
        V1_DATA_OFFSET,
        V1_NUM_TRACK_OFFSETS,
        V1_TRACK_OFFSETS_SIZE,
        V1_NUM_INSTRUMENT_OFFSETS,
        V1_OFFSET_START,
        V1_DATA_HEADER_SIZE
    };

    const ADLFile::meta_version_t mv2 = {
        V2_NUM_HEADER,
        V2_HEADER_SIZE,
        V2_DATA_OFFSET,
        V2_NUM_TRACK_OFFSETS,
        V2_TRACK_OFFSETS_SIZE,
        V2_NUM_INSTRUMENT_OFFSETS,
        V2_OFFSET_START,
        V2_DATA_HEADER_SIZE
    };

    const ADLFile::meta_version_t mv3 = {
        V3_NUM_HEADER,
        V3_HEADER_SIZE,
        V3_DATA_OFFSET,
        V3_NUM_TRACK_OFFSETS,
        V3_TRACK_OFFSETS_SIZE,
        V3_NUM_INSTRUMENT_OFFSETS,
        V3_OFFSET_START,
        V3_DATA_HEADER_SIZE
    };

    ADLFile::ADLFile(const std::string& filename) : File(filename)
    {
        assertValid_(size() >= FILE_SIZE_MIN);
        detectVersion_();
        // validate version with file size
        assertValid_(size() > m_meta_version.data_offset);
        // read Header
        seek(0, std::fstream::beg);
        readHeaderFromFile_(m_meta_version.num_headers, m_read);
        // read Track Offsets
        readOffsetsFromFile_(m_meta_version.num_track_offsets, m_track_offsets, m_meta_version.header_size);
        // read Instrument Offsets
        readOffsetsFromFile_(
            m_meta_version.num_instrument_offsets,
            m_instrument_offsets,
            m_meta_version.track_offsets_size + m_meta_version.header_size
        );

        readDataFromFile_(m_meta_version.data_offset, m_meta_version.data_header_size);

        // TODO: count_loop_ for version 3 is wrong to be 8 bit i think
        m_num_tracks = count_loop_<uint8_t>(0, m_header);
        m_num_track_offsets = count_loop_<uint16_t>(m_meta_version.offset_start, m_track_offsets);
        m_num_instrument_offsets = count_loop_<uint16_t>(m_meta_version.offset_start, m_instrument_offsets);

        // Closing file
        close();

        // Adjust Offsets
        adjust_offsets_(m_track_offsets);
        adjust_offsets_(m_instrument_offsets);
    }

    uint8_t ADLFile::getVersion() const noexcept
    {
        return m_version;
    }

    int ADLFile::getNumTracks() const noexcept
    {
        return m_num_tracks;
    }

    int ADLFile::getNumTrackOffsets() const noexcept
    {
        return m_num_track_offsets;
    }

    int ADLFile::getNumInstrumentOffsets() const noexcept
    {
        return m_num_instrument_offsets;
    }

    uint8_t ADLFile::getTrack(const int track) const
    {
        return m_header.at(track);
    }

    uint16_t ADLFile::getTrackOffset(const int programId) const
    {
        return m_track_offsets.at(programId);
    }

    uint16_t ADLFile::getInstrumentOffset(const int instrument) const
    {
        return m_instrument_offsets.at(instrument);
    }

    uint16_t ADLFile::getProgramOffset(const int progId, const PROG_TYPE prog_type) const
    {
        switch (prog_type)
        {
        case PROG_TYPE::Track:
            return getTrackOffset(progId);
        case PROG_TYPE::Instrument:
            return getInstrumentOffset(progId);
        default:
            // unreachable code
            return 0xFFFF;
        }
    }

    uint32_t ADLFile::getDataSize() const noexcept
    {
        return m_dataSize;
    }

    std::shared_ptr<uint8_t[]> ADLFile::getData() const noexcept
    {
        return m_data;
    }

    hardware::opl::OPL2instrument_t ADLFile::getInstrument(const int instrument) const
    {
        using hardware::opl::OPL2instrument_t;

        const uint8_t* instr = m_data.get() + getInstrumentOffset(instrument);
        return OPL2instrument_t(
            {
                .trem_vibr_1 = instr[0],
                .att_dec_1 = instr[7],
                .sust_rel_1 = instr[9],
                .wave_1 = instr[3],
                .scale_1 = 0, // ???
                .level_1 = instr[5],
                .feedback = instr[2],
                .trem_vibr_2 = instr[1],
                .att_dec_2 = instr[8],
                .sust_rel_2 = instr[10],
                .wave_2 = instr[4],
                .scale_2 = 0, // ???
                .level_2 = instr[6],
                .unused = 0,
                .basenote = 0 /// ???
            }
        );
    }

    void ADLFile::detectVersion_()
    {
        seek(0, std::fstream::beg);
        // detect version 3
        m_version = 3;
        m_meta_version = mv3;
        m_read = [this]() {return readLE16(); };
        for (int i = 0; i < V1_HEADER_SIZE; i++)
        {
            uint16_t v = readLE16();
            // For v3 all entries should be below 500 or equal 0xFFFF.
            // For other versions
            // this will check program offsets (at least 600).
            if ((v >= V3_HEADER_SIZE) && (v < 0xFFFF))
            {
                //v1,2
                m_version = 2;
                m_meta_version = mv2;
                m_read = [this]() {return readU8(); };
                break;
            }
        }

        // detect version 1,2
        if (m_version < 3)
        {
            seek(V1_HEADER_SIZE, std::fstream::beg);
            // validate v1
            uint16_t min_w = 0xFFFF;
            for (int i = 0; i < V1_NUM_TRACK_OFFSETS; i++)
            {
                uint16_t w = readLE16();
                assertValid_(w >= V1_OFFSET_START || w == 0);
                if (min_w > w && w > 0) {
                    min_w = w;
                }
            }
            // detect
            if (min_w < V2_OFFSET_START)
            {
                m_version = 1;
                m_meta_version = mv1;
                assertValid_(min_w == V1_OFFSET_START);
            }
            else
            {
                assertValid_(min_w == V2_OFFSET_START);
            }
        }
    }

    void ADLFile::readHeaderFromFile_(const int header_size, std::function<uint16_t()> read)
    {
        m_header.resize(header_size);
        // TODO: pass the byte size instead of a function read and read at once,
        //       and do read in LE when is greter than 1, basically if version 3
        for (int i = 0; i < header_size; i++)
        {
            m_header[i] = read();
        }

        assertValid_(m_header.size() == header_size);
    }

    void ADLFile::readOffsetsFromFile_(const int num_offsets, std::vector<uint16_t>& vec, const int offset_start) const noexcept
    {
        assertValid_(tell() == offset_start);
        vec.resize(num_offsets);
        for (int i = 0; i < num_offsets; i++) {
            vec[i] = readLE16();
        }
        assertValid_(vec.size() == num_offsets);
    }

    void ADLFile::readDataFromFile_(const int data_offsets, const int data_heder_size)
    {
        m_dataSize = size() - data_offsets;
        assertValid_(m_dataSize > 0);
        assertValid_(tell() == data_offsets);
        m_data.reset(new uint8_t[m_dataSize]);
        read(m_data.get(), m_dataSize);
        m_dataHeaderSize = data_heder_size;
    }

    void ADLFile::adjust_offsets_(std::vector<uint16_t>& vec)
    {
        for (auto& v : vec)
        {
            if (v == 0)
                v = 0xFFFF;

            if (v == 0xFFFF)
                continue;

            assertValid_(v >= m_dataHeaderSize);
            v -= m_dataHeaderSize;
        }
    }
}
