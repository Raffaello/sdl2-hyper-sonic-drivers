#include <drivers/MIDParser.hpp>
#include <spdlog/spdlog.h>
#include <vector>
#include <utils/algorithms.hpp>
#include <string>
#include <chrono>
#include <fmt/chrono.h>
#include <thread>

namespace drivers
{
    using utils::decode_VLQ;
    using utils::powerOf2;
    using utils::delayMicro;
    using audio::midi::MIDI_FORMAT;

    std::string midi_event_to_string(const std::vector<uint8_t>& e)
    {
        return std::string(++e.begin(), e.end());
    }

    MIDParser::MIDParser(std::shared_ptr<audio::MIDI> midi, std::shared_ptr<audio::scummvm::Mixer> mixer)
        : _midi(midi), _mixer(mixer)
    {
    }

    MIDParser::~MIDParser()
    {
    }

    void MIDParser::processTrack(audio::midi::MIDITrack& track, const int i, std::shared_ptr<RtMidiOut> midiout)
    {
        std::vector<uint8_t> midi_msg;
        int cur_time = 0; // ticks
        unsigned int tempo_micros = static_cast<unsigned int>(static_cast<float>(tempo) / static_cast<float>(division));
        // TODO: the timer should be inside the MIDI object when play it?
        //track.reset();
        unsigned int start = utils::getMicro<unsigned int>();
        for (auto& e : track.events)
        {
            midi_msg.clear();
            //unsigned int start = utils::getMicro<unsigned int>();
            spdlog::debug("MIDI Track#={:3}, Event: dt={:4}, type={:#04x}", i, e.delta_time, e.type.val);
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
                        tempo_micros = static_cast<unsigned int>(static_cast<float>(tempo) / static_cast<float>(division));
                        spdlog::debug("Tempo {}, ({} bpm) -- microseconds/tick", tempo, 60000000 / tempo, tempo_micros);
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
                spdlog::debug("Channel #{} Note OFF: note={}, velocity={}", (int)e.type.low, e.data[0], e.data[1]);
                midi_msg.push_back(e.type.val);
                midi_msg.push_back(e.data[0]);
                midi_msg.push_back(e.data[1]);
                midiout->sendMessage(&midi_msg);
                break;
            case 0x9: // note on
                spdlog::debug("Channel #{} Note ON: note={}, velocity={}", (int)e.type.low, e.data[0], e.data[1]);
                midi_msg.push_back(e.type.val);
                midi_msg.push_back(e.data[0]);
                midi_msg.push_back(e.data[1]);
                midiout->sendMessage(&midi_msg);
                break;
            case 0xA: // note aftertouch
                spdlog::debug("Channel #{} Note Aftertouch: note={}, ater touch value={}", (int)e.type.low, e.data[0], e.data[1]);
                midi_msg.push_back(e.type.val);
                midi_msg.push_back(e.data[0]);
                midi_msg.push_back(e.data[1]);
                midiout->sendMessage(&midi_msg);
                break;
            case 0xB: // controller
                spdlog::debug("Channel #{} Controller: number={}, value={}", (int)e.type.low, e.data[0], e.data[1]);
                midi_msg.push_back(e.type.val);
                midi_msg.push_back(e.data[0]);
                midi_msg.push_back(e.data[1]);
                midiout->sendMessage(&midi_msg);
                break;
            case 0xE: // pitch bend
                spdlog::debug("Channel #{} Pitch Bend: value={}", (int)e.type.low, (e.data[0]) | (e.data[1] << 8));
                midi_msg.push_back(e.type.val);
                midi_msg.push_back(e.data[0]);
                midi_msg.push_back(e.data[1]);
                midiout->sendMessage(&midi_msg);
                break;
                // 1 data values
            case 0xC: // program change
                spdlog::debug("Channel #{} Program change: number={}", (int)e.type.low, e.data[0]);
                midi_msg.push_back(e.type.val);
                midi_msg.push_back(e.data[0]);
                midiout->sendMessage(&midi_msg);
                break;
            case 0xD: // channel aftertouch
                spdlog::debug("Channel #{} Channel after touch: value={}", (int)e.type.low, e.data[0]);
                midi_msg.push_back(e.type.val);
                midi_msg.push_back(e.data[0]);
                midiout->sendMessage(&midi_msg);
                break;
            default:
                spdlog::critical("event type={:#04x} parsing not implemented", (int)e.type.low, e.type.val);
                return;
            }


            if (e.delta_time == 0)
                continue;

            cur_time += e.delta_time;
            const unsigned int end = utils::getMicro<unsigned int>();
            const unsigned int delta_delay = tempo_micros * e.delta_time;
            const long dd = static_cast<long>(delta_delay - (end - start));
            start = end;
            
            if (dd > 0) {
                delayMicro(dd); // not precise
                start += dd;
            }
            else {
                spdlog::warn("#{} --- cur_time={}, delta_delay={}, micro_delay_time={}", i, cur_time, delta_delay, dd);
            }
        }
    }

    void MIDParser::display(std::shared_ptr<RtMidiOut> midiout)
    {
        if (_midi->format != MIDI_FORMAT::SINGLE_TRACK) {
            spdlog::critical("MIDI format single track only supported");
            return;
        }

        num_tracks = _midi->numTracks;
        division = _midi->division;
        // TODO: division to update after processed is missing.
        if (division & 0x8000) {
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
                spdlog::warn("Division SMPTE not known = {}", smpte);
            }

            spdlog::debug("Division: Ticks per frame = {}, smpte", ticksPerFrame, smpte);
            spdlog::warn("not immplemented divsion ticks per frame yet");
        }
        else {
            // ticks per quarter note
            spdlog::debug("Division: Ticks per quarter note = {}", division & 0x7FFF);
        }

        // TODO time signature used for what?

        //std::vector<audio::midi::MIDI_track_t> tracks = _mid_file->getTracks();
        tempo = 500000; //120 BPM;

        
        auto start_time = std::chrono::system_clock::now();

        for (int i = 0; i < num_tracks; i++)
        {
            audio::midi::MIDITrack track = _midi->getTrack(i);
            processTrack(track, i, midiout);
        }

        // TODO: this works only with a constant tempo during all the sequence
        // BODY: also should be computed in float and ceiled for integer.
        float exp_time_seconds = static_cast<float>(_midi->getMaxTicks()) / static_cast<float>(division) * (static_cast<float>(tempo) / 1000000.0f);
        auto end_time = std::chrono::system_clock::now();
        auto tot_time = end_time - start_time;
        spdlog::info("Total Running Time: {:%M:%S}, expected={}:{}",
            tot_time,
            std::floor(exp_time_seconds / 60.0f),
            exp_time_seconds - (std::floor(exp_time_seconds / 60.f) * 60.f));
    }
}
