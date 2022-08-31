#include <files/MIDFile.hpp>
#include <string>
#include <utils/endianness.hpp>
#include <memory>
#include <spdlog/spdlog.h>
#include <cstring>
#include <utils/algorithms.hpp>
#include <audio/midi/MIDITrack.hpp>
#include <audio/midi/MIDIEvent.hpp>
#include <algorithm>


namespace files
{
    using audio::midi::MIDI_FORMAT;

    // TODO consider to "join" with IFFFile / or put common functions altogheter
    //      as it is similar to just the sub_header_chunk of IFF file.
    constexpr const char* MIDI_HEADER = "MThd";
    constexpr const char* MIDI_TRACK = "MTrk";

    MIDFile::MIDFile(const std::string& filename) : File(filename),
        _midi(nullptr)
    {
        read_header();
        check_format();

        // Read Tracks
        for (int i = 0; i < _midi->numTracks; i++)
        {
            read_track();
        }
    }

    MIDFile::~MIDFile() = default;

    std::shared_ptr<audio::MIDI> MIDFile::getMIDI() const noexcept
    {
        return _midi;
    }

    std::shared_ptr<audio::MIDI> MIDFile::convertToSingleTrackMIDI() const
    {
        if (_midi->format == audio::midi::MIDI_FORMAT::SINGLE_TRACK)
            return getMIDI();
        if (_midi->format == audio::midi::MIDI_FORMAT::MULTI_TRACK)
            throw std::runtime_error("MIDI MULTI_TRACK not supported yet");

        auto midi = std::make_shared<audio::MIDI>(audio::midi::MIDI_FORMAT::SINGLE_TRACK, 1, _midi->division);
        // uint32_t is abs_time
        typedef std::vector<std::pair<audio::midi::MIDIEvent, uint32_t>> VecPairs;
        VecPairs events_pair;
        
        //std::vector<audio::midi::MIDIEvent> events_;
        uint32_t abs_time = 0;
        // 1. with absolute time just copy all the events as they are into 1 single track
        for (uint16_t n = 0; n < _midi->numTracks; n++)
        {
            abs_time = 0;
            for (const auto& te : _midi->getTrack(n).getEvents()) {
                abs_time += te.delta_time;
                events_pair.push_back(std::make_pair(te, abs_time));
                //events_.push_back(te);
            }
        }

        // 2. then sort them by absolute time
        // TODO: sort is "unsorting" the events like sequence text
        //       all at the same abs_time, delta_time 0
        //       and those must be kept in the same order as when visualizing is the only way to keep them in the same
        //       order, more exactly each track as a sequence name and the order is implicit based on track number.
        //       so i can order by 2nd level track number as a tie break on abs_time
        std::sort(
            events_pair.begin(),
            events_pair.end(),
            [](const std::pair<audio::midi::MIDIEvent, uint32_t>& e1, const std::pair<audio::midi::MIDIEvent, uint32_t>& e2)
            {
                return e1.second < e2.second;
            }
        );

        /*std::sort(
            events_.begin(),
            events_.end(),
            [](const audio::midi::MIDIEvent& e1, const audio::midi::MIDIEvent& e2)
            {
                return e1.abs_time < e2.abs_time;
            }
        );*/

        // 3. recompute delta time from absolute time
        abs_time = 0;
        for (auto& e : events_pair)
        {
            e.first.delta_time = e.second - abs_time;
            if (e.second > abs_time)
                abs_time = e.second;
        }
        /*abs_time = 0;
        for (auto& e : events_)
        {
            e.delta_time = e.abs_time - abs_time;
            if (e.abs_time > abs_time)
                abs_time = e.abs_time;
        }*/

        // 4. extract MIDITrack from events without abs_time
        std::vector<audio::midi::MIDIEvent> events;
        std::transform(events_pair.begin(),
            events_pair.end(),
            std::back_inserter(events),
            std::bind(&VecPairs::value_type::first, std::placeholders::_1));


        // 5. DEBUG, compare _events with _events
        /*std::vector<std::pair<audio::midi::MIDIEvent, audio::midi::MIDIEvent>> v3;
        {
            auto it1 = events.begin();
            auto it2 = _events.begin();
            auto end1 = events.end();
            auto end2 = _events.end();

            while (it1 != end1 && it2 != end2) {
                if (it1->abs_time != it2->abs_time) {
                    v3.push_back(std::make_pair(*it1, *it2));
                }
                ++it1;
                ++it2;
            }

            if (!v3.empty()) {
                int ah = !0;
            }
        }*/

        audio::midi::MIDITrack single_track(events);
        midi->addTrack(single_track);
        assert(midi->getTrack().getEvents().size() == single_track.getEvents().size());

        return midi;
    }

    int MIDFile::decode_VLQ(uint32_t& out_value)
    {
        uint8_t buf[4] = { 0, 0, 0, 0 };
        uint8_t i = 0;
        uint8_t v = 0;
        
        do {
            buf[i++] = v = readU8();
            _assertValid(i <= 4);
        } while (v & 0x80);

        return utils::decode_VLQ(buf, out_value);
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
        _assertValid(strncmp(header.id, MIDI_HEADER, sizeof(header.id)) == 0);
        _assertValid(header.length == 6);


        uint16_t format = readBE16();
        uint16_t nTracks = readBE16();
        uint16_t division = readBE16();

        _midi = std::make_shared<audio::MIDI>(static_cast<MIDI_FORMAT>(format), nTracks, division);
    }

    void MIDFile::check_format()
    {
        switch (_midi->format)
        {
        case MIDI_FORMAT::SINGLE_TRACK:
            _assertValid(_midi->numTracks == 1);
            break;
        case MIDI_FORMAT::SIMULTANEOUS_TRACK:
        case MIDI_FORMAT::MULTI_TRACK:
            _assertValid(_midi->numTracks >= 1);
            break;
        default:
            throw std::runtime_error("MIDFile: _format invalid");
        }
    }

    void MIDFile::read_track()
    {
        using audio::midi::MIDI_EVENT_type_u;
        using audio::midi::MIDI_META_EVENT;
        using audio::midi::MIDITrack;
        using audio::midi::MIDIEvent;
        using audio::midi::MIDI_EVENT_TYPES_HIGH;
        using audio::midi::MIDI_META_EVENT_TYPES_LOW;

        MIDITrack track;
        bool endTrack = false;
        MIDI_EVENT_type_u lastStatus = { 0 };
        // Read Track
        midi_chunk_t chunk = read_chunk();
        _assertValid(strncmp(chunk.id, MIDI_TRACK, sizeof(chunk.id)) == 0);

        // events
        int offs = 0;
        uint32_t abs_time = 0;
        uint32_t prev_abs_time = 0;
        while (!endTrack)
        {
            // MTrck Event:
            MIDIEvent e;
            // delta time encoded in VRQ
            offs += decode_VLQ(e.delta_time);
            abs_time += e.delta_time;
            if (prev_abs_time > abs_time)
            {
                // uint32_t abs_time overflow
                // it shoulnd't happen in "small midi" files
                // but just a sanity check just in case...
                // to figure it out when it might happen.
                // in theory never for small midi files.
                throw std::runtime_error("MIDI file too long, absolute time overflowed");
            }

            //e.abs_time = abs_time;
            e.type.val = readU8();

            if (e.type.high < 0x8) {
                e.type = lastStatus;
                seek(-1, std::fstream::cur);
            }
            else {
                offs++;
            }

            switch (static_cast<MIDI_EVENT_TYPES_HIGH>(e.type.high))
            {
            case MIDI_EVENT_TYPES_HIGH::META_SYSEX:
                // special event
                switch (static_cast<MIDI_META_EVENT_TYPES_LOW>(e.type.low))
                {
                case MIDI_META_EVENT_TYPES_LOW::SYS_EX0:
                    // sysEx-event
                    spdlog::error("sysEx event not implemented yet");
                    break;
                    case MIDI_META_EVENT_TYPES_LOW::SYS_EX7:
                    // sysEx-event
                    spdlog::error("sysEx event not implemented yet");
                    break;
                case MIDI_META_EVENT_TYPES_LOW::META:
                {
                    // meta-event
                    uint8_t type = readU8();
                    offs++;
                    _assertValid(type < 128);
                    uint32_t length = 0;
                    offs += decode_VLQ(length);
                    e.data.reserve(length + 1);
                    e.data.push_back(type);
                    for (int j = 0; j < length; j++) {
                        e.data.push_back(readU8());
                        offs++;
                    }

                    if (MIDI_META_EVENT::END_OF_TRACK == static_cast<MIDI_META_EVENT>(type)) {
                        endTrack = true;
                    }
                    break;
                }
                default:
                    spdlog::critical("MIDFile sub-event {:#04x} not recognized", e.type.val);
                    throw std::runtime_error("");

                }
                break;
            case MIDI_EVENT_TYPES_HIGH::PROGRAM_CHANGE:
            case MIDI_EVENT_TYPES_HIGH::CHANNEL_AFTERTOUCH:
                e.data.reserve(1);
                e.data.push_back(readU8());
                offs++;
                break;
            case MIDI_EVENT_TYPES_HIGH::NOTE_OFF:
            case MIDI_EVENT_TYPES_HIGH::NOTE_ON:
            case MIDI_EVENT_TYPES_HIGH::AFTERTOUCH:
            case MIDI_EVENT_TYPES_HIGH::CONTROLLER:
            case MIDI_EVENT_TYPES_HIGH::PITCH_BEND:
                e.data.reserve(2);
                e.data.push_back(readU8());
                e.data.push_back(readU8());
                offs += 2;
                break;
            default:
                // using previous status
                if (lastStatus.val == 0) {

                    spdlog::critical("MIDFile: midi event {:#04x} not recognized {:#03x} - last status = {} (pos={}).", e.type.val, (uint8_t)e.type.high, lastStatus.val, tell());
                    throw std::runtime_error("MIDFile: midi event type not recognized.");
                }
            }

            e.data.shrink_to_fit();
            track.addEvent(e);
            lastStatus = e.type;
        }

        // sanity check
        if (offs != chunk.length) {
            spdlog::warn("MIDFile: Fileanme '{}' track {} length mismatch real length {}", _filename, chunk.length, offs);
        }

        track.lock();
        _midi->addTrack(track);
    }
}
