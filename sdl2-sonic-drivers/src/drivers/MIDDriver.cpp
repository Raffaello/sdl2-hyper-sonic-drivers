#include <drivers/MIDDriver.hpp>
#include <spdlog/spdlog.h>
#include <utils/algorithms.hpp>
#include <array>
#include <thread>

namespace drivers
{
    constexpr unsigned int tempo_to_micros(const uint32_t tempo, const uint16_t division)
    {
        return static_cast<unsigned int>(static_cast<float>(tempo) / static_cast<float>(division));
    }

    MIDDriver::MIDDriver(std::shared_ptr<audio::scummvm::Mixer> mixer, std::shared_ptr<midi::Device> device)
        : _mixer(mixer), _device(device)
    {
    }
    
    void MIDDriver::play(const std::shared_ptr<audio::MIDI> midi) const noexcept
    {
        using audio::midi::MIDI_FORMAT;

        if (midi->format != MIDI_FORMAT::SINGLE_TRACK && midi->numTracks != 1) {
            spdlog::critical("MIDI format single track only supported");
            return;
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

        processTrack(midi->getTrack(), midi->division);
    }

    void MIDDriver::processTrack(const audio::midi::MIDITrack& track, const uint16_t division) const noexcept
    {
        using audio::midi::MIDI_EVENT_TYPES_HIGH;

        uint32_t tempo = 500000; //120 BPM;
        int cur_time = 0; // ticks
        unsigned int tempo_micros = tempo_to_micros(tempo, division);
        spdlog::debug("tempo_micros = {}", tempo_micros);
        unsigned int start = utils::getMicro<unsigned int>();
        const auto& tes = track.getEvents();
        std::array<uint8_t, 3> msg = {};
        uint8_t msg_size = 0;
        for (const auto& e : tes)
        {
            switch (static_cast<MIDI_EVENT_TYPES_HIGH>(e.type.high))
            {
            case MIDI_EVENT_TYPES_HIGH::META:
                if (e.type.low == 0xF)
                {
                    const uint8_t type = e.data[0]; // must be < 128
                    if (static_cast<audio::midi::MIDI_META_EVENT>(type) == audio::midi::MIDI_META_EVENT::SET_TEMPO)
                    {
                        const int skip = 1;
                        tempo = (e.data[skip] << 16) + (e.data[skip + 1] << 8) + (e.data[skip + 2]);
                        tempo_micros = tempo_to_micros(tempo, division);
                        spdlog::debug("Tempo {}, ({} bpm) -- microseconds/tick {}", tempo, 60000000 / tempo, tempo_micros);
                    }
                }
                continue;
            case MIDI_EVENT_TYPES_HIGH::NOTE_OFF:
            case MIDI_EVENT_TYPES_HIGH::NOTE_ON:
            case MIDI_EVENT_TYPES_HIGH::AFTERTOUCH:
            case MIDI_EVENT_TYPES_HIGH::CONTROLLER:
            case MIDI_EVENT_TYPES_HIGH::PITCH_BEND:
                msg[0] = e.type.val;
                msg[1] = e.data[0];
                msg[2] = e.data[1];
                msg_size = 3;
                break;
            case MIDI_EVENT_TYPES_HIGH::PROGRAM_CHANGE:
            case MIDI_EVENT_TYPES_HIGH::CHANNEL_AFTERTOUCH:
                msg[0] = e.type.val;
                msg[1] = e.data[0];
                msg_size = 2;
                break;
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
                    spdlog::warn("cur_time={}, delta_delay={}, micro_delay_time={}", cur_time, delta_delay, dd);
                }
            }

            _device->sendMessage(msg.data(), msg_size);
            //_device->sendEvent(e);
        }
    }
}
