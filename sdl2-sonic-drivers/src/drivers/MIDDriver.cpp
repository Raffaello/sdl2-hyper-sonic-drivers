#include <drivers/MIDDriver.hpp>
#include <spdlog/spdlog.h>
#include <utils/algorithms.hpp>
#include <array>
#include <thread>

namespace drivers
{
    constexpr int DEFAULT_MIDI_TEMPO = 500000;
    constexpr int PAUSE_MILLIS = 100;
    constexpr unsigned int tempo_to_micros(const uint32_t tempo, const uint16_t division)
    {
        return static_cast<unsigned int>(static_cast<float>(tempo) / static_cast<float>(division));
    }

    MIDDriver::MIDDriver(std::shared_ptr<audio::scummvm::Mixer> mixer, std::shared_ptr<midi::Device> device)
        : _mixer(mixer), _device(device)
    {
    }

    MIDDriver::~MIDDriver()
    {
        stop();
    }
    
    void MIDDriver::play(const std::shared_ptr<audio::MIDI> midi) noexcept
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

        stop();
        _isPlaying = true;
        _player = std::thread(&MIDDriver::processTrack, this, midi->getTrack(), midi->division & 0x7FFF);
    }

    void MIDDriver::stop() noexcept
    {
        _force_stop = true;
        _paused = false;
        if(_player.joinable())
            _player.join();
        _force_stop = false;
        _isPlaying = false;
    }

    void MIDDriver::pause() noexcept
    {
        if (_isPlaying)
            _paused = true;
    }

    void MIDDriver::resume() noexcept
    {
        if (_isPlaying)
            _paused = false;
    }

    bool MIDDriver::isPlaying() const noexcept
    {
        return _isPlaying;
    }

    bool MIDDriver::isPaused() const noexcept
    {
        return _paused;
    }

    void MIDDriver::processTrack(const audio::midi::MIDITrack& track, const uint16_t division)
    {
        using audio::midi::MIDI_EVENT_TYPES_HIGH;
        using audio::midi::MIDI_META_EVENT_TYPES_LOW;
        using audio::midi::MIDI_META_EVENT;

        _isPlaying = true;
        uint32_t tempo = DEFAULT_MIDI_TEMPO; //120 BPM;
        int cur_time = 0; // ticks
        unsigned int tempo_micros = tempo_to_micros(tempo, division);
        spdlog::debug("tempo_micros = {}", tempo_micros);
        unsigned int start = utils::getMicro<unsigned int>();
        const auto& tes = track.getEvents();
        
        for (const auto& e : tes)
        {
            std::array<uint8_t, 3> msg = {};
            uint8_t msg_size = 0;
            while(_paused) {
                utils::delayMillis(PAUSE_MILLIS);
                start = utils::getMicro<unsigned int>();
            }

            if (_force_stop)
                break;

            switch (static_cast<MIDI_EVENT_TYPES_HIGH>(e.type.high))
            {
            case MIDI_EVENT_TYPES_HIGH::META_SYSEX:
            {
                switch (static_cast<MIDI_META_EVENT_TYPES_LOW>(e.type.low))
                {
                case MIDI_META_EVENT_TYPES_LOW::META: {
                    const uint8_t type = e.data[0]; // must be < 128
                    switch (static_cast<audio::midi::MIDI_META_EVENT>(type))
                    {
                    case MIDI_META_EVENT::SET_TEMPO: {
                        const int skip = 1;
                        tempo = (e.data[skip] << 16) + (e.data[skip + 1] << 8) + (e.data[skip + 2]);
                        tempo_micros = tempo_to_micros(tempo, division);
                        spdlog::debug("Tempo {}, ({} bpm) -- microseconds/tick {}", tempo, 60000000 / tempo, tempo_micros);
                        break;
                    }
                    case MIDI_META_EVENT::SEQUENCE_NAME: {
                        std::string name = utils::midi_event_to_string(++(e.data.begin()), e.data.end());
                        spdlog::info("SEQUENCE NAME: {}", name);

                        break;
                    }
                    default: {
                        spdlog::error("MIDI_META_EVENT_TYPES_LOW not recognized: {:#02x}", type);
                        break;
                    }
                    }

                    continue; // META event processed, go on next MIDI event
                    //break;
                }
                case MIDI_META_EVENT_TYPES_LOW::SYS_EX0: {
                    spdlog::debug("SYS_EX0 META event...");
                    // First byte length encoded in VLQ, remaining data is the sysEx data
                    //uint8_t vlq_length = e.data[0];

                    _device->sendSysEx(e);
                    continue;
                }
                case MIDI_META_EVENT_TYPES_LOW::SYS_EX7: {
                    spdlog::debug("SYS_EX7 META event...");
                    _device->sendSysEx(e);
                    continue;
                    //break;
                }
                default: {
                    break;
                }
                }
                break;
            }
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
            default:
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

        _isPlaying = false;
    }
}
