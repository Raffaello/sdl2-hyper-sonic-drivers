#include <files/MIDFile.hpp>
#include <string>
#include <utils/endianness.hpp>
#include <memory>



namespace files
{
    constexpr int32_t MAX_VRQ = 0x0FFFFFFF; // 3.5 bytes = 27 bits

    // TODO consider to "join" with IFFFile / or put common functions altogheter
    //      as it is similar to just the sub_header_chunk of IFF file.
    constexpr const char* _MIDI_HEADER = "MThd";
    constexpr const char* _MIDI_TRACK = "MTrk";
    const std::string MIDI_HEADER = std::string(_MIDI_HEADER);
    const std::string MIDI_TRACK = std::string(_MIDI_TRACK);

    MIDFile::MIDFile(const std::string& filename) : File(filename)
    {
        // TODO VLQ

        read_header();
        check_format();

        // Read Tracks
        for (int i = 0; i < _nTracks; i++)
        {
            // Read Track
            midi_chunk_t chunk = read_chunk();
            _assertValid(MIDI_TRACK.compare(chunk.id));
            auto buf = std::make_unique<uint8_t[]>(chunk.length);
            read(buf.get(), chunk.length);
            // delta time encoded in VRQ
            uint8_t byte = 0;
            uint32_t delta_time = 0;
            do {
                byte = readU8();
                delta_time = (delta_time << 7) | (byte & 0x7F);
            } while (byte & 0x80);
            //
            int j = 0;


        }
    }

    MIDFile::~MIDFile()
    {}

    uint32_t MIDFile::decode_VLQ(const uint8_t buf[4])
    {
        int i = 0;
        uint8_t byte = 0;
        uint32_t value = 0;
        do
        {
            if (i >= 4) {
                throw std::runtime_error("MIDFile: more than 32bits VLQ input");
            }

            byte = buf[i++];
            value = (value << 7) | (byte & 0x7F);
        } while (byte & 0x80);

        return value;
    }

    MIDFile::midi_chunk_t MIDFile::read_chunk()
    {
        midi_chunk_t chunk;

        read(&chunk, sizeof(midi_chunk_t));
        chunk.length = utils::swapBE32(chunk.length);

        return chunk;
    }

    void MIDFile::read_header()
    {
        midi_chunk_t header = read_chunk();
        _assertValid(MIDI_HEADER.compare(header.id));
        _assertValid(header.length == 6);

        _format = readBE16();
        _nTracks = readBE16();
        _division = readBE16();
    }

    void MIDFile::check_format()
    {
        switch (static_cast<eFormat>(_format))
        {
        case eFormat::SINGLE_TRACK:
            _assertValid(_nTracks == 1);
            break;
        case eFormat::SIMULTANEOUS_TRACK:
            _assertValid(_nTracks >= 1);
            break;
        case eFormat::MULTI_TRACK:
            _assertValid(_nTracks >= 1);
            break;
        default:
            throw std::runtime_error("MIDFile: _format invalid");
        }
    }
}
