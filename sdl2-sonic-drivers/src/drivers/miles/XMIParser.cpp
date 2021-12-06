#include <audio/midi/types.hpp>
#include <drivers/miles/XMIParser.hpp>
#include <utils/algorithms.hpp>
#include <spdlog/spdlog.h>
#include <fmt/chrono.h>
#include <vector>

namespace drivers
{
    namespace miles
    {
        using utils::decode_xmi_VLQ;
        using utils::decode_VLQ;
        using utils::powerOf2;
        using utils::delayMicro;
        
        std::string midi_event_to_string(const std::vector<uint8_t>& e)
        {
            return std::string(++e.begin(), e.end());
        }

        XMIParser::XMIParser(std::shared_ptr<audio::MIDI> midi, std::shared_ptr<audio::scummvm::Mixer> mixer)
            : _midi(midi), _mixer(mixer), tempo(0)
        {
        }

        XMIParser::~XMIParser()
        {
        }

        void XMIParser::display(const int num_track)
        {
            // TODO: Note that note_on and note_off are different from midi.

            // TODO: division to update after processed is missing.
            if (_midi->division & 0x8000) {
                // ticks per frame
                int smpte = (_midi->division & 0x7FFF) >> 8;
                int ticksPerFrame = _midi->division & 0xFF;
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

                spdlog::debug("Division: Ticks per frame = {}, smpte", ticksPerFrame, smpte);
                spdlog::warn("not immplemented divsion ticks per frame yet");
            }
            else {
                // ticks per quarter note
                spdlog::debug("Division: Ticks per quarter note = {}", _midi->division & 0x7FFF);
            }

            auto start_time = std::chrono::system_clock::now();
            audio::midi::MIDITrack track = _midi->getTrack(num_track);
            //track.setTempo(500000);
            tempo = 500000; //120 BPM;
            int cur_time = 0; // ticks
            unsigned int start = utils::getMicro<unsigned int>();
            for (auto& e : track.events)
            {
                //unsigned int start = utils::getMicro<unsigned int>();
                spdlog::debug("MIDI Track#={:3}, Event: dt={:4}, type={:#04x}", num_track, e.delta_time, e.type.val);
                switch (e.type.high)
                {
                case 0xF:
                    switch (e.type.low)
                    {
                    case 0xF: // meta-event
                    {
                        uint8_t type = e.data[0]; // must be < 128
                        uint32_t length = e.data.size() - 1; // previously decoded

                        int skip = 1;

                        switch (static_cast<audio::midi::MIDI_META_EVENT>(type))
                        {
                        case audio::midi::MIDI_META_EVENT::CHANNEL_PREFIX:
                            spdlog::debug("Channel {}", e.data[skip]); // 0-15
                            break;
                        case audio::midi::MIDI_META_EVENT::COPYRIGHT:
                            spdlog::debug("Copyright {}", &e.data[skip]);
                            break;
                        case audio::midi::MIDI_META_EVENT::CUE_POINT:
                            spdlog::debug("Cue Point {}", &e.data[skip]);
                            break;
                        case audio::midi::MIDI_META_EVENT::END_OF_TRACK:
                            spdlog::debug("End Of Track");
                            break;
                        case audio::midi::MIDI_META_EVENT::INSTRUMENT_NAME:
                            spdlog::debug("Instrument Name {}", &e.data[skip]);
                            break;
                        case audio::midi::MIDI_META_EVENT::KEY_SIGNATURE:
                            spdlog::debug("Key: {:d},  major/minor={:d}", e.data[skip], e.data[skip + 1]);
                            break;
                        case audio::midi::MIDI_META_EVENT::LYRICS:
                            spdlog::debug("Lyrics {}", &e.data[skip]);
                            break;
                        case audio::midi::MIDI_META_EVENT::MARKER:
                            spdlog::debug("Marker {}", &e.data[skip]);
                            break;
                        case audio::midi::MIDI_META_EVENT::MIDI_PORT:
                            spdlog::debug("Midi Port {:d}", e.data[skip]);
                            break;
                        case audio::midi::MIDI_META_EVENT::SEQUENCER_SPECIFIC:
                            spdlog::warn("Sequencer specific, skip");
                            break;
                        case audio::midi::MIDI_META_EVENT::SEQUENCE_NAME:
                        {
                            spdlog::debug("Sequence Name '{}'", midi_event_to_string(e.data));
                            break;
                        }
                        case audio::midi::MIDI_META_EVENT::SEQUENCE_NUMBER:
                            spdlog::debug("Sequence Number {:d} {:d}", e.data[skip + 1], e.data[skip + 2]);
                            break;
                        case audio::midi::MIDI_META_EVENT::SET_TEMPO:
                        {
                            tempo = (e.data[skip] << 16) + (e.data[skip + 1] << 8) + (e.data[skip + 2]);
                            spdlog::debug("Tempo {}, ({} bpm)", tempo, 60000000 / tempo);
                            break;
                        }
                        case audio::midi::MIDI_META_EVENT::SMPTE_OFFSET:
                            spdlog::debug("SMPTE OFFSET hh={:d}, mm={:d}, ss={:d}, frames={:d}, fractional_frame={:d}",
                                e.data[skip], e.data[skip + 1], e.data[skip + 2], e.data[skip + 3], e.data[skip + 4]);
                            break;
                        case audio::midi::MIDI_META_EVENT::TEXT:
                            spdlog::debug("Text {}", midi_event_to_string(e.data));
                            break;
                        case audio::midi::MIDI_META_EVENT::TIME_SIGNATURE:
                        {
                            spdlog::info("Time Signature: {:d}/{:d}, midi_clock={}, ticks per metronome click={}",
                                e.data[skip], powerOf2(e.data[skip + 1]), e.data[skip + 2], e.data[skip + 3]);
                            break;
                        }
                        default:
                            spdlog::error("MetaEvent not know");
                            return;
                        }
                    }
                    break;
                    case 0x0:
                    case 0x7:
                        spdlog::warn("SysEx Event type={:#04x}, skipped", e.type.val);
                    }
                    break;

                    // 2 data values
                case 0x8: // note off
                    spdlog::critical("Channel #{} Note OFF:");
                    return;
                case 0x9: // note on
                    spdlog::debug("Channel #{} Note ON: note={}, velocity={}, duration={}", (int)e.type.low, e.data[0], e.data[1], e.data[2]);
                    break;
                case 0xA: // note aftertouch
                    spdlog::debug("Channel #{} Note Aftertouch: note={}, ater touch value={}", (int)e.type.low, e.data[0], e.data[1]);
                    break;
                case 0xB: // controller
                    spdlog::debug("Channel #{} Controller: number={}, value={}", (int)e.type.low, e.data[0], e.data[1]);
                    break;
                case 0xE: // pitch bend
                    spdlog::debug("Channel #{} Pitch Bend: value={}", (int)e.type.low, (e.data[0]) | (e.data[1] << 8));
                    break;
                    // 1 data values
                case 0xC: // program change
                    spdlog::debug("Channel #{} Program change: number={}", (int)e.type.low, e.data[0]);
                    break;
                case 0xD: // channel aftertouch
                    spdlog::debug("Channel #{} Channel after touch: value={}", (int)e.type.low, e.data[0]);
                    break;
                default:
                    spdlog::critical("event type={:#04x} parsing not implemented", (int)e.type.low, e.type.val);
                    return;
                }

                cur_time += e.delta_time;
                //if (cur_time > _ticks)
                {
                    float tt = static_cast<float>(tempo) / static_cast<float>(_midi->division);
                    unsigned int end = utils::getMicro<unsigned int>();
                    float delta_delay = e.delta_time * tt;
                    float dd = std::roundf(delta_delay - (end - start));
                    start = end;
                    spdlog::info("#{} --- cur_time={}, delta_delay={}, dd={}, end-start={}", num_track, cur_time, delta_delay, dd, delta_delay - dd);
                    if (dd >= 1.0f) {
                        delayMicro(dd); // not precise
                        start += dd;
                    }

                }
            }

            // TODO: this works only with a constant tempo during all the sequence
            // BODY: also should be computed in float and ceiled for integer.
            float exp_time_seconds = static_cast<float>(_midi->getMaxTicks()) / static_cast<float>(_midi->division) * (static_cast<float>(tempo) / 1000000.0f);
            auto end_time = std::chrono::system_clock::now();
            auto tot_time = end_time - start_time;
            spdlog::info("Total Running Time: {:%M:%S}, expected={}:{}",
                tot_time,
                std::floor(exp_time_seconds / 60.0f),
                exp_time_seconds - (std::floor(exp_time_seconds / 60.f) * 60.f));
        }


        // Each track is 1 sequence (song), like Format 0 in midi files
        // So it is required to choose which track to play
        void XMIParser::displayAllTracks()
        {
            // TODO: not sure if it is ok the way to interpret the tracks.
            spdlog::set_level(spdlog::level::debug);

            for (int it = 0; it < _midi->numTracks; it++)
            {
                display(it);
            }
        }
    }
}
