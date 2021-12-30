#include <drivers/MIDDriver.hpp>
#include <spdlog/spdlog.h>
#include <cstdint>
#include <utils/algorithms.hpp>

namespace drivers
{
    using audio::midi::MIDI_FORMAT;

    constexpr unsigned int tempo_to_micros(const uint32_t tempo, const uint16_t division) {
        return static_cast<unsigned int>(static_cast<float>(tempo) / static_cast<float>(division));
    }

    MIDDriver::MIDDriver(std::shared_ptr<audio::scummvm::Mixer> mixer): _mixer(mixer)
    {
    }

    bool MIDDriver::playMidi(const std::shared_ptr<audio::MIDI> midi)
    {
        if (midi->format != MIDI_FORMAT::SINGLE_TRACK && midi->numTracks != 1) {
            spdlog::critical("MIDI format single track only supported");
            return false;
        }

        if (midi->division & 0x8000)
        {
            // ticks per frame
            int smpte = (midi->division & 0x7FFF) >> 8;
            int ticksPerFrame = midi->division & 0xFF;
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
        else
        {
            // ticks per quarter note
            spdlog::debug("Division: Ticks per quarter note = {}", midi->division & 0x7FFF);
        }

        // TODO time signature used for what?

        uint32_t tempo = 500000; //120 BPM;
        int cur_time = 0; // ticks
        unsigned int tempo_micros = tempo_to_micros(tempo, midi->division);
        unsigned int start = utils::getMicro<unsigned int>();
        
        for (auto& e : midi->getTrack(0).events)
        {
            //std::vector<uint8_t> midi_msg;
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
                        //spdlog::debug("Channel {}", e.data[skip]); // 0-15
                        break;
                    case audio::midi::MIDI_META_EVENT::COPYRIGHT:
                        //spdlog::debug("Copyright {}", &e.data[skip]);
                        break;
                    case audio::midi::MIDI_META_EVENT::CUE_POINT:
                        //spdlog::debug("Cue Point {}", &e.data[skip]);
                        break;
                    case audio::midi::MIDI_META_EVENT::END_OF_TRACK:
                        //spdlog::debug("End Of Track");
                        break;
                    case audio::midi::MIDI_META_EVENT::INSTRUMENT_NAME:
                        //spdlog::debug("Instrument Name {}", &e.data[skip]);
                        break;
                    case audio::midi::MIDI_META_EVENT::KEY_SIGNATURE:
                        //spdlog::debug("Key: {:d},  major/minor={:d}", e.data[skip], e.data[skip + 1]);
                        break;
                    case audio::midi::MIDI_META_EVENT::LYRICS:
                        //spdlog::debug("Lyrics {}", &e.data[skip]);
                        break;
                    case audio::midi::MIDI_META_EVENT::MARKER:
                        //spdlog::debug("Marker {}", &e.data[skip]);
                        break;
                    case audio::midi::MIDI_META_EVENT::MIDI_PORT:
                        //spdlog::debug("Midi Port {:d}", e.data[skip]);
                        break;
                    case audio::midi::MIDI_META_EVENT::SEQUENCER_SPECIFIC:
                        //spdlog::warn("Sequencer specific, skip");
                        break;
                    case audio::midi::MIDI_META_EVENT::SEQUENCE_NAME:
                    {
                        //spdlog::debug("Sequence Name '{}'", utils::midi_event_to_string(e.data));
                        break;
                    }
                    case audio::midi::MIDI_META_EVENT::SEQUENCE_NUMBER:
                        //spdlog::debug("Sequence Number {:d} {:d}", e.data[skip + 1], e.data[skip + 2]);
                        break;
                    case audio::midi::MIDI_META_EVENT::SET_TEMPO:
                    {
                        tempo = (e.data[skip] << 16) + (e.data[skip + 1] << 8) + (e.data[skip + 2]);
                        tempo_micros = tempo_to_micros(tempo, midi->division);
                        spdlog::debug("Tempo {}, ({} bpm) -- microseconds/tick", tempo, 60000000 / tempo, tempo_micros);
                        break;
                    }
                    case audio::midi::MIDI_META_EVENT::SMPTE_OFFSET:
                        //spdlog::debug("SMPTE OFFSET hh={:d}, mm={:d}, ss={:d}, frames={:d}, fractional_frame={:d}",
                        //    e.data[skip], e.data[skip + 1], e.data[skip + 2], e.data[skip + 3], e.data[skip + 4]);
                        break;
                    case audio::midi::MIDI_META_EVENT::TEXT:
                        //spdlog::debug("Text {}", utils::midi_event_to_string(e.data));
                        break;
                    case audio::midi::MIDI_META_EVENT::TIME_SIGNATURE:
                    {
                        //spdlog::info("Time Signature: {:d}/{:d}, midi_clock={}, ticks per metronome click={}",
                        //    e.data[skip], utils::powerOf2(e.data[skip + 1]), e.data[skip + 2], e.data[skip + 3]);
                        break;
                    }
                    default:
                        //spdlog::error("MetaEvent not know");
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
                //midiout->sendMessage(&midi_msg);
                break;
            case 0x9: // note on
                spdlog::debug("Channel #{} Note ON: note={}, velocity={}", (int)e.type.low, e.data[0], e.data[1]);
                midi_msg.push_back(e.type.val);
                midi_msg.push_back(e.data[0]);
                midi_msg.push_back(e.data[1]);
                //midiout->sendMessage(&midi_msg);
                break;
            case 0xA: // note aftertouch
                spdlog::debug("Channel #{} Note Aftertouch: note={}, ater touch value={}", (int)e.type.low, e.data[0], e.data[1]);
                midi_msg.push_back(e.type.val);
                midi_msg.push_back(e.data[0]);
                midi_msg.push_back(e.data[1]);
                //midiout->sendMessage(&midi_msg);
                break;
            case 0xB: // controller
                spdlog::debug("Channel #{} Controller: number={}, value={}", (int)e.type.low, e.data[0], e.data[1]);
                midi_msg.push_back(e.type.val);
                midi_msg.push_back(e.data[0]);
                midi_msg.push_back(e.data[1]);
                //midiout->sendMessage(&midi_msg);
                break;
            case 0xE: // pitch bend
                spdlog::debug("Channel #{} Pitch Bend: value={}", (int)e.type.low, (e.data[0]) | (e.data[1] << 8));
                midi_msg.push_back(e.type.val);
                midi_msg.push_back(e.data[0]);
                midi_msg.push_back(e.data[1]);
                //midiout->sendMessage(&midi_msg);
                break;
                // 1 data values
            case 0xC: // program change
                spdlog::debug("Channel #{} Program change: number={}", (int)e.type.low, e.data[0]);
                midi_msg.push_back(e.type.val);
                midi_msg.push_back(e.data[0]);
                //midiout->sendMessage(&midi_msg);
                break;
            case 0xD: // channel aftertouch
                spdlog::debug("Channel #{} Channel after touch: value={}", (int)e.type.low, e.data[0]);
                midi_msg.push_back(e.type.val);
                midi_msg.push_back(e.data[0]);
                //midiout->sendMessage(&midi_msg);
                break;
            default:
                spdlog::critical("event type={:#04x} parsing not implemented", (int)e.type.low, e.type.val);
                return;
            }

            if (e.delta_time != 0)
            {
                cur_time += e.delta_time;
                const unsigned int delta_delay = tempo_micros * e.delta_time;
                const unsigned int end = utils::getMicro<unsigned int>();
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

            if (midi_msg.size() > 0)
                midiout->sendMessage(&midi_msg);
        }
    }
}
