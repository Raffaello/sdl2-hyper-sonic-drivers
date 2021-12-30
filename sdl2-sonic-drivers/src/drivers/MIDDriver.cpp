#include <drivers/MIDDriver.hpp>
#include <spdlog/spdlog.h>
#include <utils/algorithms.hpp>
#include <array>
#include <thread>

namespace drivers
{
    constexpr unsigned int tempo_to_micros(const uint32_t tempo, const uint16_t division) {
        return static_cast<unsigned int>(static_cast<float>(tempo) / static_cast<float>(division));
    }

    bool MIDDriver::playMidi(const std::shared_ptr<audio::MIDI> midi, midi::Device& device)
    {
        using audio::midi::MIDI_FORMAT;

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

        processTrack(midi->getTrack(0), device, midi->division);
        //uint32_t tempo = 500000; //120 BPM;
        //int cur_time = 0; // ticks
        //unsigned int tempo_micros = tempo_to_micros(tempo, midi->division);
        //spdlog::debug("tempo_micros = {}", tempo_micros);
        //unsigned int start = utils::getMicro<unsigned int>();
        //const auto& tes = midi->getTrack(0).events;
        //std::array<uint8_t, 3> msg = {};
        //int msg_size = 0;
        //for (const auto& e : tes)
        //{
        //    //switch (e.type.high)
        //    //{
        //    //case 0xF:
        //    //    if (e.type.low == 0xF)
        //    //    {
        //    //        const uint8_t type = e.data[0]; // must be < 128
        //    //        if (static_cast<audio::midi::MIDI_META_EVENT>(type) == audio::midi::MIDI_META_EVENT::SET_TEMPO)
        //    //        {
        //    //            const uint32_t length = e.data.size() - 1; // previously decoded
        //    //            const int skip = 1;
        //    //            tempo = (e.data[skip] << 16) + (e.data[skip + 1] << 8) + (e.data[skip + 2]);
        //    //            tempo_micros = tempo_to_micros(tempo, midi->division);
        //    //            spdlog::debug("Tempo {}, ({} bpm) -- microseconds/tick {}", tempo, 60000000 / tempo, tempo_micros);
        //    //        }
        //    //        continue;
        //    //    }
        //    //    break;

        //    //case 0x8:
        //    //case 0x9:
        //    //case 0xA:
        //    //case 0xB:
        //    //case 0xE:
        //    //    msg[0] = e.type.val;
        //    //    msg[1] = e.data[0];
        //    //    msg[2] = e.data[1];
        //    //    //device.sendMessage(msg.data(), 3);
        //    //    break;
        //    //case 0xC:
        //    //case 0xD:
        //    //    msg[0] = e.type.val;
        //    //    msg[1] = e.data[0];
        //    //    //device.sendMessage(msg.data(), 2);
        //    //}


        //    switch (e.type.high)
        //    {
        //    case 0xF:
        //        switch (e.type.low)
        //        {
        //        case 0xF: // meta-event
        //        {
        //            uint8_t type = e.data[0]; // must be < 128
        //            uint32_t length = e.data.size() - 1; // previously decoded

        //            int skip = 1;

        //            switch (static_cast<audio::midi::MIDI_META_EVENT>(type))
        //            {
        //            case audio::midi::MIDI_META_EVENT::CHANNEL_PREFIX:
        //                break;
        //            case audio::midi::MIDI_META_EVENT::COPYRIGHT:
        //                break;
        //            case audio::midi::MIDI_META_EVENT::CUE_POINT:
        //                break;
        //            case audio::midi::MIDI_META_EVENT::END_OF_TRACK:
        //                break;
        //            case audio::midi::MIDI_META_EVENT::INSTRUMENT_NAME:
        //                break;
        //            case audio::midi::MIDI_META_EVENT::KEY_SIGNATURE:
        //                break;
        //            case audio::midi::MIDI_META_EVENT::LYRICS:
        //                break;
        //            case audio::midi::MIDI_META_EVENT::MARKER:
        //                break;
        //            case audio::midi::MIDI_META_EVENT::MIDI_PORT:
        //                break;
        //            case audio::midi::MIDI_META_EVENT::SEQUENCER_SPECIFIC:
        //                break;
        //            case audio::midi::MIDI_META_EVENT::SEQUENCE_NAME:
        //            {
        //                break;
        //            }
        //            case audio::midi::MIDI_META_EVENT::SEQUENCE_NUMBER:
        //                break;
        //            case audio::midi::MIDI_META_EVENT::SET_TEMPO:
        //            {
        //                tempo = (e.data[skip] << 16) + (e.data[skip + 1] << 8) + (e.data[skip + 2]);
        //                tempo_micros = static_cast<unsigned int>(static_cast<float>(tempo) / static_cast<float>(midi->division));
        //                spdlog::debug("Tempo {}, ({} bpm) -- microseconds/tick", tempo, 60000000 / tempo, tempo_micros);
        //                break;
        //            }
        //            case audio::midi::MIDI_META_EVENT::SMPTE_OFFSET:
        //                break;
        //            case audio::midi::MIDI_META_EVENT::TEXT:
        //                break;
        //            case audio::midi::MIDI_META_EVENT::TIME_SIGNATURE:
        //            {
        //                break;
        //            }
        //            default:
        //                spdlog::error("MetaEvent not know");
        //                return false;
        //            }
        //        }
        //        break;
        //        case 0x0:
        //        case 0x7:
        //            spdlog::warn("SysEx Event type={:#04x}, skipped", e.type.val);
        //        }
        //        break;

        //        // 2 data values
        //    case 0x8: // note off
        //        spdlog::debug("Channel #{} Note OFF: note={}, velocity={}", (int)e.type.low, e.data[0], e.data[1]);
        //        msg[0] = e.type.val;
        //        msg[1] = e.data[0];
        //        msg[2] = e.data[1];
        //        //    //device.sendMessage(msg.data(), 3);
        //        //midiout->sendMessage(&midi_msg);
        //        break;
        //    case 0x9: // note on
        //        spdlog::debug("Channel #{} Note ON: note={}, velocity={}", (int)e.type.low, e.data[0], e.data[1]);
        //        msg[0] = e.type.val;
        //        msg[1] = e.data[0];
        //        msg[2] = e.data[1];
        //        //midiout->sendMessage(&midi_msg);
        //        break;
        //    case 0xA: // note aftertouch
        //        spdlog::debug("Channel #{} Note Aftertouch: note={}, ater touch value={}", (int)e.type.low, e.data[0], e.data[1]);
        //        msg[0] = e.type.val;
        //        msg[1] = e.data[0];
        //        msg[2] = e.data[1];
        //        //midiout->sendMessage(&midi_msg);
        //        break;
        //    case 0xB: // controller
        //        spdlog::debug("Channel #{} Controller: number={}, value={}", (int)e.type.low, e.data[0], e.data[1]);
        //        msg[0] = e.type.val;
        //        msg[1] = e.data[0];
        //        msg[2] = e.data[1];
        //        //midiout->sendMessage(&midi_msg);
        //        break;
        //    case 0xE: // pitch bend
        //        spdlog::debug("Channel #{} Pitch Bend: value={}", (int)e.type.low, (e.data[0]) | (e.data[1] << 8));
        //        msg[0] = e.type.val;
        //        msg[1] = e.data[0];
        //        msg[2] = e.data[1];
        //        //midiout->sendMessage(&midi_msg);
        //        break;
        //        // 1 data values
        //    case 0xC: // program change
        //        spdlog::debug("Channel #{} Program change: number={}", (int)e.type.low, e.data[0]);
        //        msg[0] = e.type.val;
        //        msg[1] = e.data[0];
        //        //midiout->sendMessage(&midi_msg);
        //        break;
        //    case 0xD: // channel aftertouch
        //        spdlog::debug("Channel #{} Channel after touch: value={}", (int)e.type.low, e.data[0]);
        //        msg[0] = e.type.val;
        //        msg[1] = e.data[0];
        //        //midiout->sendMessage(&midi_msg);
        //        break;
        //    default:
        //        spdlog::critical("event type={:#04x} parsing not implemented", (int)e.type.low, e.type.val);
        //        return false;
        //    }

        //    if (e.delta_time != 0)
        //    {
        //        cur_time += e.delta_time;
        //        const unsigned int delta_delay = tempo_micros * e.delta_time;
        //        const unsigned int end = utils::getMicro<unsigned int>();
        //        const long dd = static_cast<long>(delta_delay - (end - start));
        //        start = end;

        //        if (dd > 0) {
        //            utils::delayMicro(dd);
        //            start += dd;
        //        }
        //        else {
        //            spdlog::warn("cur_time={}, delta_delay={} (end-start)={}, micro_delay_time={}", cur_time, delta_delay, (delta_delay-dd), dd);
        //        }
        //    }

        //    if (msg_size > 0)
        //        device.sendMessage(msg.data(), msg_size);
        //}

        //return true;
    }

    void MIDDriver::processTrack(const audio::midi::MIDITrack& track, midi::Device& device, const uint16_t division)
    {
        uint32_t tempo = 500000; //120 BPM;
        int cur_time = 0; // ticks
        unsigned int tempo_micros = tempo_to_micros(tempo, division);
        spdlog::debug("tempo_micros = {}", tempo_micros);
        unsigned int start = utils::getMicro<unsigned int>();
        const auto& tes = track.events;
        std::array<uint8_t, 3> msg = {};
        int msg_size = 0;
        for (const auto& e : tes)
        {
            //switch (e.type.high)
            //{
            //case 0xF:
            //    if (e.type.low == 0xF)
            //    {
            //        const uint8_t type = e.data[0]; // must be < 128
            //        if (static_cast<audio::midi::MIDI_META_EVENT>(type) == audio::midi::MIDI_META_EVENT::SET_TEMPO)
            //        {
            //            const uint32_t length = e.data.size() - 1; // previously decoded
            //            const int skip = 1;
            //            tempo = (e.data[skip] << 16) + (e.data[skip + 1] << 8) + (e.data[skip + 2]);
            //            tempo_micros = tempo_to_micros(tempo, midi->division);
            //            spdlog::debug("Tempo {}, ({} bpm) -- microseconds/tick {}", tempo, 60000000 / tempo, tempo_micros);
            //        }
            //        continue;
            //    }
            //    break;

            //case 0x8:
            //case 0x9:
            //case 0xA:
            //case 0xB:
            //case 0xE:
            //    msg[0] = e.type.val;
            //    msg[1] = e.data[0];
            //    msg[2] = e.data[1];
            //    //device.sendMessage(msg.data(), 3);
            //    break;
            //case 0xC:
            //case 0xD:
            //    msg[0] = e.type.val;
            //    msg[1] = e.data[0];
            //    //device.sendMessage(msg.data(), 2);
            //}


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
                        break;
                    case audio::midi::MIDI_META_EVENT::COPYRIGHT:
                        break;
                    case audio::midi::MIDI_META_EVENT::CUE_POINT:
                        break;
                    case audio::midi::MIDI_META_EVENT::END_OF_TRACK:
                        break;
                    case audio::midi::MIDI_META_EVENT::INSTRUMENT_NAME:
                        break;
                    case audio::midi::MIDI_META_EVENT::KEY_SIGNATURE:
                        break;
                    case audio::midi::MIDI_META_EVENT::LYRICS:
                        break;
                    case audio::midi::MIDI_META_EVENT::MARKER:
                        break;
                    case audio::midi::MIDI_META_EVENT::MIDI_PORT:
                        break;
                    case audio::midi::MIDI_META_EVENT::SEQUENCER_SPECIFIC:
                        break;
                    case audio::midi::MIDI_META_EVENT::SEQUENCE_NAME:
                    {
                        break;
                    }
                    case audio::midi::MIDI_META_EVENT::SEQUENCE_NUMBER:
                        break;
                    case audio::midi::MIDI_META_EVENT::SET_TEMPO:
                    {
                        tempo = (e.data[skip] << 16) + (e.data[skip + 1] << 8) + (e.data[skip + 2]);
                        tempo_micros = static_cast<unsigned int>(static_cast<float>(tempo) / static_cast<float>(division));
                        spdlog::debug("Tempo {}, ({} bpm) -- microseconds/tick", tempo, 60000000 / tempo, tempo_micros);
                        break;
                    }
                    case audio::midi::MIDI_META_EVENT::SMPTE_OFFSET:
                        break;
                    case audio::midi::MIDI_META_EVENT::TEXT:
                        break;
                    case audio::midi::MIDI_META_EVENT::TIME_SIGNATURE:
                    {
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
                msg[0] = e.type.val;
                msg[1] = e.data[0];
                msg[2] = e.data[1];
                //    //device.sendMessage(msg.data(), 3);
                //midiout->sendMessage(&midi_msg);
                break;
            case 0x9: // note on
                spdlog::debug("Channel #{} Note ON: note={}, velocity={}", (int)e.type.low, e.data[0], e.data[1]);
                msg[0] = e.type.val;
                msg[1] = e.data[0];
                msg[2] = e.data[1];
                //midiout->sendMessage(&midi_msg);
                break;
            case 0xA: // note aftertouch
                spdlog::debug("Channel #{} Note Aftertouch: note={}, ater touch value={}", (int)e.type.low, e.data[0], e.data[1]);
                msg[0] = e.type.val;
                msg[1] = e.data[0];
                msg[2] = e.data[1];
                //midiout->sendMessage(&midi_msg);
                break;
            case 0xB: // controller
                spdlog::debug("Channel #{} Controller: number={}, value={}", (int)e.type.low, e.data[0], e.data[1]);
                msg[0] = e.type.val;
                msg[1] = e.data[0];
                msg[2] = e.data[1];
                //midiout->sendMessage(&midi_msg);
                break;
            case 0xE: // pitch bend
                spdlog::debug("Channel #{} Pitch Bend: value={}", (int)e.type.low, (e.data[0]) | (e.data[1] << 8));
                msg[0] = e.type.val;
                msg[1] = e.data[0];
                msg[2] = e.data[1];
                //midiout->sendMessage(&midi_msg);
                break;
                // 1 data values
            case 0xC: // program change
                spdlog::debug("Channel #{} Program change: number={}", (int)e.type.low, e.data[0]);
                msg[0] = e.type.val;
                msg[1] = e.data[0];
                //midiout->sendMessage(&midi_msg);
                break;
            case 0xD: // channel aftertouch
                spdlog::debug("Channel #{} Channel after touch: value={}", (int)e.type.low, e.data[0]);
                msg[0] = e.type.val;
                msg[1] = e.data[0];
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
                    utils::delayMicro(dd);
                    start += dd;
                }
                else {
                    spdlog::warn("cur_time={}, delta_delay={} (end-start)={}, micro_delay_time={}", cur_time, delta_delay, (delta_delay - dd), dd);
                }
            }

            if (msg_size > 0)
                device.sendMessage(msg.data(), msg_size);
        }
    }
}
