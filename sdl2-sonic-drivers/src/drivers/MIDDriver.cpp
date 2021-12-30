#include <drivers/MIDDriver.hpp>
#include <spdlog/spdlog.h>
#include <utils/algorithms.hpp>

namespace drivers
{
    constexpr unsigned int tempo_to_micros(const uint32_t tempo, const uint16_t division) {
        return static_cast<unsigned int>(static_cast<float>(tempo) / static_cast<float>(division));
    }

    bool MIDDriver::playMidi(const std::shared_ptr<audio::MIDI> midi, const midi::Device& device)
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

        uint32_t tempo = 500000; //120 BPM;
        int cur_time = 0; // ticks
        unsigned int tempo_micros = tempo_to_micros(tempo, midi->division);
        unsigned int start = utils::getMicro<unsigned int>();

        for (auto& e : midi->getTrack(0).events)
        {
            if (e.delta_time != 0)
            {
                cur_time += e.delta_time;
                const unsigned int delta_delay = tempo_micros * e.delta_time;
                const unsigned int end = utils::getMicro<unsigned int>();
                const long dd = static_cast<long>(delta_delay - (end - start));
                start = end;

                if (dd > 0) {
                    utils::delayMicro(dd); // not precise
                    start += dd;
                }
                else {
                    spdlog::warn("cur_time={}, delta_delay={}, micro_delay_time={}", cur_time, delta_delay, dd);
                }
            }

            if (e.type.val == 0xFF)
            {
                const uint8_t type = e.data[0]; // must be < 128
                const uint32_t length = e.data.size() - 1; // previously decoded
                const int skip = 1;
                switch (static_cast<audio::midi::MIDI_META_EVENT>(type))
                {
                case audio::midi::MIDI_META_EVENT::SET_TEMPO:
                    tempo = (e.data[skip] << 16) + (e.data[skip + 1] << 8) + (e.data[skip + 2]);
                    tempo_micros = tempo_to_micros(tempo, midi->division);
                    spdlog::debug("Tempo {}, ({} bpm) -- microseconds/tick", tempo, 60000000 / tempo, tempo_micros);
                    break;
                }
            } else 
                device.sendEvent(e);
        }

        return true;
    }
}
