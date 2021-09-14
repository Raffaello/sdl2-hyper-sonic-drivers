#include <drivers/MIDParser.hpp>
#include <spdlog/spdlog.h>
#include <vector>
#include <utils/algorithms.hpp>
#include <string>
#include <cstdint>

namespace drivers
{
    using files::MIDFile;
    using utils::decode_VLQ;
    using utils::powerOf2;


    std::string midi_event_to_string(const std::vector<uint8_t>& e)
    {
        return std::string(++e.begin(), e.end());
    }

    MIDParser::MIDParser(std::shared_ptr<files::MIDFile> mid_file, std::shared_ptr<audio::scummvm::Mixer> mixer)
        : _mid_file(mid_file), _mixer(mixer)
    {
    }

    MIDParser::~MIDParser()
    {
    }

    void MIDParser::display()
    {
        if (_mid_file->getFormat() == 2) {
            spdlog::info("MIDI format 2 not supported");
            return;
        }

        int num_tracks = _mid_file->getNumTracks();
        int division = _mid_file->getDivision();
        if (division & 0x8000) {
            // ticks per quarter note
            spdlog::info("Division: Ticks per quarter note = {}", division & 0x7FFF);
        }
        else {
            // ticks per frame
            int smpte = (division & 0x7FFF) >> 8;
            int ticksPerFrame = division & 0xFF;
            switch (smpte)
            {
            case -24:
            case -25:
            case -29:
            case -30:
                break;
            default: 
                spdlog::warn("Division SMPTE not know = {}", smpte);
            }

            spdlog::info("Division: Ticks per frame = {}, smpte", ticksPerFrame, smpte);

        }

        std::vector<MIDFile::MIDI_track_t> tracks = _mid_file->getTracks();

        for (int i = 0; i < num_tracks; i++)
        {
            MIDFile::MIDI_track_t track = tracks[i];
            for (auto& e : track.events)
            {
                spdlog::info("MIDI Track#={:3}, Event: dt={:4}, type={:#04x}",i, e.delta_time, e.type.val);
                switch (e.type.val)
                {
                case 0xFF: // meta-event
                {
                    uint8_t type = e.data[0]; // must be < 128
                    uint32_t length = e.data.size() - 1; // previously decoded, but don't know how many bytes to skip!
                    // sanity check
                    //uint32_t out_length;
                    //int skip = decode_VLQ(&e.data[1], out_length) + 1; // +1 because of the [0] index = type
                    //if (length != out_length) {
                    //    spdlog::error("Meta-Event length error: expected {} but is {}", length, out_length);
                    //}
                    int skip = 1;
                    
                    switch (static_cast<MIDFile::MIDI_META_EVENT>(type))
                    {
                    case MIDFile::MIDI_META_EVENT::CHANNEL_PREFIX:
                        spdlog::info("Channel {}", e.data[skip]); // 0-15
                        break;
                    case MIDFile::MIDI_META_EVENT::COPYRIGHT:
                        spdlog::info("Copyright {}", &e.data[skip]);
                        break;
                    case MIDFile::MIDI_META_EVENT::CUE_POINT:
                        spdlog::info("Cue Point {}", &e.data[skip]);
                        break;
                    case MIDFile::MIDI_META_EVENT::END_OF_TRACK:
                        spdlog::info("End Of Track");
                        break;
                    case MIDFile::MIDI_META_EVENT::INSTRUMENT_NAME:
                        spdlog::info("Instrument Name {}", &e.data[skip]);
                        break;
                    case MIDFile::MIDI_META_EVENT::KEY_SIGNATURE:
                        spdlog::info("Key: {:d},  major/minor={:d}", e.data[skip], e.data[skip + 1]);
                        break;
                    case MIDFile::MIDI_META_EVENT::LYRICS:
                        spdlog::info("Lyrics {}", &e.data[skip]);
                        break;
                    case MIDFile::MIDI_META_EVENT::MARKER:
                        spdlog::info("Marker {}", &e.data[skip]);
                        break;
                    case MIDFile::MIDI_META_EVENT::MIDI_PORT:
                        spdlog::info("Midi Port {:d}", e.data[skip]);
                        break;
                    case MIDFile::MIDI_META_EVENT::SEQUENCER_SPECIFIC:
                        spdlog::warn("Sequencer specific, skip");
                        break;
                    case MIDFile::MIDI_META_EVENT::SEQUENCE_NAME:
                    {
                        spdlog::info("Sequence Name '{}'", midi_event_to_string(e.data));
                        break;
                    }
                    case MIDFile::MIDI_META_EVENT::SEQUENCE_NUMBER:
                        spdlog::info("Sequence Number {:d} {:d}", e.data[skip + 1], e.data[skip + 2]);
                        break;
                    case MIDFile::MIDI_META_EVENT::SET_TEMPO:
                    {
                        uint32_t tempo = (e.data[skip] << 16) + (e.data[skip + 1] << 8) + (e.data[skip + 2]);
                        spdlog::info("Tempo {}, ({} bps)", tempo, 60000000/tempo);
                        break;
                    }
                    case MIDFile::MIDI_META_EVENT::SMPTE_OFFSET:
                        spdlog::info("SMPTE OFFSET hh={:d}, mm={:d}, ss={:d}, frames={:d}, fractional_frame={:d}",
                            e.data[skip], e.data[skip + 1], e.data[skip + 2], e.data[skip + 3], e.data[skip + 4]);
                        break;
                    case MIDFile::MIDI_META_EVENT::TEXT:
                        spdlog::info("Text {}", midi_event_to_string(e.data));
                        break;
                    case MIDFile::MIDI_META_EVENT::TIME_SIGNATURE:
                    {
                        spdlog::info("Time Signature: {:d}/{:d}, midi_clock={}, bb={}",
                            e.data[skip], powerOf2(e.data[skip + 1]), e.data[skip + 2], e.data[skip + 3]);
                        break;
                    }
                    default: 
                        spdlog::error("MetaEvent not know");
                        return;
                    }

                    break;
                }
                default:
                    spdlog::critical("event type={} parsing not implemented", e.type.val);
                    return;
                }
            }
        }
    }
}
