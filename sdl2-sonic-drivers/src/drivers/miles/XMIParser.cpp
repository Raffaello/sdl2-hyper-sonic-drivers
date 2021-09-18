#include <drivers/miles/XMIParser.hpp>
#include <vector>
#include <spdlog/spdlog.h>
#include <utils/algorithms.hpp>
#include <audio/midi/types.hpp>

namespace drivers
{
    namespace miles
    {
        using utils::decode_xmi_VLQ;
        using utils::decode_VLQ;
        using utils::powerOf2;
        

        XMIParser::XMIParser(std::shared_ptr<files::miles::XMIFile> xmi_file, std::shared_ptr<audio::scummvm::Mixer> mixer)
            : _xmi_file(xmi_file), _mixer(mixer), tempo(0)
        {
            num_tracks = _xmi_file->getNumTracks();
        }

        XMIParser::~XMIParser()
        {
        }

        void XMIParser::display(const int num_track)
        {
            std::vector<uint8_t> track = _xmi_file->getTrack(num_track);

            // process track
            // TODO need to be processed byte by byte ...
            // BODY: might have sense to have it in XMIFile
            bool endOftrack = false;
            int i = 0;
            while (!endOftrack)
            {
                // delta time
                uint32_t dt = 0;
                i += decode_xmi_VLQ(&track[i], dt);

                uint8_t t = track[i++];
                if (t == 0xFF)
                {
                    uint8_t type = track[i++];
                    // decode_xmi_vlq
                    uint32_t length = 0;
                    //i += decode_xmi_VLQ(track, i, length);
                    i += decode_VLQ(&track[i], length);
                    switch (static_cast<audio::midi::MIDI_META_EVENT>(type))
                    {
                    case audio::midi::MIDI_META_EVENT::CHANNEL_PREFIX:
                        spdlog::debug("Channel {}", &track[i]); // 0-15
                        break;
                    case audio::midi::MIDI_META_EVENT::COPYRIGHT:
                        spdlog::debug("Copyright {}", &track[i]);
                        break;
                    case audio::midi::MIDI_META_EVENT::CUE_POINT:
                        spdlog::debug("Cue Point {}", &track[i]);
                        break;
                    case audio::midi::MIDI_META_EVENT::END_OF_TRACK:
                        spdlog::debug("End Of Track, valid={}", (bool)length == 0);
                        if (i % 2 && i < track.size()) {
                            // 1 spare byte?
                            spdlog::debug("End Of Track pad byte, valid={}", (bool)track[i++] == 0);
                        }
                        endOftrack = true;
                        break;
                    case audio::midi::MIDI_META_EVENT::INSTRUMENT_NAME:
                        spdlog::debug("Instrument Name {}", &track[i]);
                        break;
                    case audio::midi::MIDI_META_EVENT::KEY_SIGNATURE:
                        spdlog::debug("Key: {:d},  major/minor={:d}", track[i], track[i + 1]);
                        break;
                    case audio::midi::MIDI_META_EVENT::LYRICS:
                        spdlog::debug("Lyrics {}", &track[i]);
                        break;
                    case audio::midi::MIDI_META_EVENT::MARKER:
                        spdlog::debug("Marker {}", &track[i]);
                        break;
                    case audio::midi::MIDI_META_EVENT::MIDI_PORT:
                        spdlog::debug("Midi Port {:d}", &track[i]);
                        break;
                    case audio::midi::MIDI_META_EVENT::SEQUENCER_SPECIFIC:
                        spdlog::warn("Sequencer specific, skip");
                        break;
                    case audio::midi::MIDI_META_EVENT::SEQUENCE_NAME:
                    {
                        spdlog::debug("Sequence Name '{}'", &track[i]);
                        break;
                    }
                    case audio::midi::MIDI_META_EVENT::SEQUENCE_NUMBER:
                        spdlog::debug("Sequence Number {:d} {:d}", track[i], track[i + 1]);
                        break;
                    case audio::midi::MIDI_META_EVENT::SET_TEMPO:
                    {
                        tempo = (track[i] << 16) + (track[i + 1] << 8) + (track[i + 2]);
                        spdlog::debug("Tempo {}, ({} bpm)", tempo, 60000000 / tempo);
                        break;
                    }
                    case audio::midi::MIDI_META_EVENT::SMPTE_OFFSET:
                        spdlog::debug("SMPTE OFFSET hh={:d}, mm={:d}, ss={:d}, frames={:d}, fractional_frame={:d}",
                            track[i], track[i + 1], track[i + 2], track[i + 3], track[i + 3]);
                        break;
                    case audio::midi::MIDI_META_EVENT::TEXT:
                        spdlog::debug("Text {}", &track[i]);
                        break;
                    case audio::midi::MIDI_META_EVENT::TIME_SIGNATURE:
                    {
                        spdlog::info("Time Signature: {:d}/{:d}, midi_clock={}, ticks per metronome click={}",
                            track[i], powerOf2(track[i + 1]), track[i + 2], track[i + 3]);
                        break;
                    }
                    default:
                        spdlog::error("MetaEvent not know");
                        return;
                    }
                    i += length;
                }
                else if (t >= 0x80)
                {
                    // MIDI Event
                    uint8_t tt = t >> 4;
                    uint8_t ch = t & 0xF;
                    switch (tt)
                    {
                    case 0x8: // note off
                    {
                        spdlog::warn("Channel #{} Note OFF? -- t={:#04x}, tt={}", (int)ch, t, tt);
                    }
                    break;
                    case 0x9:
                    {
                        // note on
                        // The first difference is "Note On" event contains 3 parameters:
                        // the note number, velocity level (same as standard MIDI),
                        // and also duration in ticks.
                        // Duration is stored as variable-length value inconcatenated bits format.
                        // Since note events store information about its duration,
                        // there are no "NoteOff" events.
                        uint8_t note = track[i++];
                        uint8_t vel = track[i++];
                        uint32_t duration = 0;
                        // TODO: figure out the VLQ here... not correct?
                        i += decode_VLQ(&track[i], duration);
                        //i+= decode_VLQ(&track[i], duration);

                        spdlog::debug("Channel #{} Note ON: note={}, velocity={}, duration={}", (int)ch, note, vel, duration);
                    }
                    break;
                    case 0xA: // note aftertouch
                        spdlog::debug("Channel #{} Note Aftertouch: note={}, ater touch value={}", (int)ch, track[i++], track[i++]);
                        break;
                    case 0xB: // controller
                        spdlog::debug("Channel #{} Controller: number={}, value={}", (int)ch, track[i++], track[i++]);
                        break;
                    case 0xE: // pitch bend
                        spdlog::debug("Channel #{} Pitch Bend: value={}", (int)ch, (track[i++]) | (track[i++] << 8));
                        break;
                        // 1 data values
                    case 0xC: // program change
                        spdlog::debug("Channel #{} Program change: number={}", (int)ch, track[i++]);
                        break;
                    case 0xD: // channel aftertouch
                        spdlog::debug("Channel #{} Channel after touch: value={}", (int)ch, track[i++]);
                        break;
                    default:
                        spdlog::critical("event type={:#04x} parsing not implemented", (int)ch, t);
                        return;
                    }
                }
                else {
                    spdlog::critical("not recognized: t={}", t);
                }
            }

            // end of track sanity check
            if (i != track.size()) {
                spdlog::error("end of track don't match track size: end of track index={}, track size={}", i, track.size());
            }
        }

        // Each track is 1 sequence (song), like Format 0 in midi files
        // So it is required to choose which track to play
        void XMIParser::displayAllTracks()
        {
            spdlog::set_level(spdlog::level::debug);

            for (int it = 0; it < num_tracks; it++)
            {
                display(it);
            }
        }
    }
}
