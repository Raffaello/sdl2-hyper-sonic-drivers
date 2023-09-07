#include <array>
#include <thread>
#include <format>
#include <HyperSonicDrivers/drivers/MIDDriver.hpp>
#include <HyperSonicDrivers/utils/algorithms.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>

namespace HyperSonicDrivers::drivers
{
    using utils::logT;
    using utils::logD;
    using utils::logI;
    using utils::logW;
    using utils::logC;

    constexpr uint32_t DEFAULT_MIDI_TEMPO = 500000;
    constexpr uint32_t PAUSE_MILLIS = 100;
    //constexpr int32_t DELAY_CHUNK_MIN_MICROS = 500 * 1000; // 500ms
    constexpr int32_t DELAY_CHUNK_MICROS = 250 * 1000; // 250ms

    constexpr uint32_t tempo_to_micros(const uint32_t tempo, const uint16_t division)
    {
        // TODO: it can be integer division? test it.
        return static_cast<uint32_t>(static_cast<float>(tempo) / static_cast<float>(division));
    }

    inline uint32_t get_start_time()
    {
        return utils::getMicro<uint32_t>();
    }

    MIDDriver::MIDDriver(/*const std::shared_ptr<audio::IMixer>& mixer,*/ const std::shared_ptr<midi::Device>& device)
        : /*_mixer(mixer),*/ m_device(device)
    {
    }

    MIDDriver::~MIDDriver()
    {
        stop();
    }
    
    void MIDDriver::play(const std::shared_ptr<audio::MIDI>& midi) noexcept
    {
        using audio::midi::MIDI_FORMAT;

        if (midi->format != MIDI_FORMAT::SINGLE_TRACK && midi->numTracks != 1)
        {
            logC("MIDI format single track supported only");
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
                logW("SMPTE not implemented yet");
                break;
            default:
                logW(std::format("Division SMPTE not known = {}", smpte));
            }

            logD(std::format("Division: Ticks per frame = {}, {}", ticksPerFrame, smpte));
            logW("division ticks per frame not implemented yet");
        }
        else
        {
            // ticks per quarter note
            logD(std::format("Division: Ticks per quarter note = {}", midi->division & 0x7FFF));
        }

        stop();
        if (!m_device->acquire(this)) {
            return;
        }

        //TODO: it would be better reusing the thread...
        m_player = std::thread(&MIDDriver::processTrack, this, midi->getTrack(), midi->division & 0x7FFF);
        m_isPlaying = true;
    }

    void MIDDriver::stop(/*const bool wait*/) noexcept
    {
        m_force_stop = true;
        m_paused = false;
        if (m_player.joinable())
            m_player.join();
        m_force_stop = false;
        m_isPlaying = false;
        m_device->release(this);
    }

    void MIDDriver::pause() noexcept
    {
        if (m_isPlaying)
            m_paused = true;
    }

    void MIDDriver::resume() noexcept
    {
        if (m_isPlaying)
            m_paused = false;
    }

    bool MIDDriver::isPlaying() const noexcept
    {
        return m_isPlaying;
    }

    bool MIDDriver::isPaused() const noexcept
    {
        return m_paused;
    }

    void MIDDriver::processTrack(const audio::midi::MIDITrack& track, const uint16_t division)
    {
        using audio::midi::MIDI_EVENT_TYPES_HIGH;
        using audio::midi::MIDI_META_EVENT_TYPES_LOW;
        using audio::midi::MIDI_META_EVENT;
        using audio::midi::MIDI_META_EVENT_VAL;
        using audio::midi::TO_HIGH;
        using audio::midi::TO_META_LOW;
        using audio::midi::TO_META;

        m_isPlaying = true;
        m_force_stop = false;
        setTempo(DEFAULT_MIDI_TEMPO); //120 BPM;
        int cur_time = 0; // ticks
        unsigned int tempo_micros = tempo_to_micros(m_tempo, division);
        logD(std::format("tempo_micros = {}", tempo_micros));
        uint32_t start = get_start_time();
        const auto& tes = track.getEvents();
        for (const auto& e : tes)
        {
            if(m_paused)
            {
                m_device->pause();
                do
                {
                    utils::delayMillis(PAUSE_MILLIS);
                } while (m_paused);
                m_device->resume();
                start = get_start_time();
            }

            if (m_force_stop)
                break;

            switch (TO_HIGH(e.type.high))
            {
            case MIDI_EVENT_TYPES_HIGH::META_SYSEX:
            {
                switch (TO_META_LOW(e.type.low))
                {
                case MIDI_META_EVENT_TYPES_LOW::META:
                {
                    const uint8_t type = e.data[0]; // must be < 128
                    std::string str;
                    switch (TO_META(type))
                    {
                    case MIDI_META_EVENT::CHANNEL_PREFIX:
                        logW(std::format("CHANNEL_PREFIX {:d} not implemented", e.data[1]));
                        break;
                    case MIDI_META_EVENT::COPYRIGHT:
                        str = utils::chars_vector_to_string_skip_first(e.data);
                        logT(std::format("CopyRight: {}", str));
                        break;
                    case MIDI_META_EVENT::CUE_POINT:
                        str = utils::chars_vector_to_string_skip_first(e.data);
                        logD(std::format("Cue Point: {}", str));
                        break;
                    case MIDI_META_EVENT::DEVICE_NAME:
                        str = utils::chars_vector_to_string_skip_first(e.data);
                        logW(std::format("[Not Implemented] Device Name: {}", str));
                        break;
                    case MIDI_META_EVENT::END_OF_TRACK:
                        logD("MIDI end of track.");
                        break;
                    case MIDI_META_EVENT::INSTRUMENT_NAME:
                        str = utils::chars_vector_to_string_skip_first(e.data);
                        logT(std::format("Instrument name: {}", str));
                        break;
                    case MIDI_META_EVENT::KEY_SIGNATURE:
                        logT(std::format("KEY_SIGNATURE: {:d} {:d}", e.data[1], e.data[2]));
                        break;
                    case MIDI_META_EVENT::LYRICS:
                        str = utils::chars_vector_to_string_skip_first(e.data);
                        logT(std::format("Lyrics: {}", str));
                        break;
                    case MIDI_META_EVENT::MARKER:
                        str = utils::chars_vector_to_string_skip_first(e.data);
                        logT(std::format("Marker: {}", str));
                        break;
                    case MIDI_META_EVENT::MIDI_PORT:
                        logW(std::format("MIDI_PORT {:d} not implemented", e.data[1]));
                        break;
                    case MIDI_META_EVENT::PROGRAM_NAME:
                        str = utils::chars_vector_to_string_skip_first(e.data);
                        logT(std::format("PROGRAM_NAME: {}", str));
                        break;
                    case MIDI_META_EVENT::SEQUENCER_SPECIFIC:
                        logW("SEQUENCE_SPECIFIC not implemented");
                        break;
                    case MIDI_META_EVENT::SEQUENCE_NAME: // a.k.a track name
                        str = utils::chars_vector_to_string(++(e.data.begin()), e.data.end());
                        logT(std::format("SEQUENCE NAME: {}", str));
                        break;
                    case MIDI_META_EVENT::SEQUENCE_NUMBER:
                        logW("Sequence number not implemented");
                        break;
                    case MIDI_META_EVENT::SET_TEMPO: {
                        setTempo((e.data[1] << 16) + (e.data[2] << 8) + (e.data[3]));
                        tempo_micros = tempo_to_micros(m_tempo, division);
                        logT(std::format("Tempo {}, ({} bpm) -- microseconds/tick {}", m_tempo.load(), 60000000 / m_tempo.load(), tempo_micros));
                        break;
                    }
                    case MIDI_META_EVENT::SMPTE_OFFSET:
                        logW("SMPTE_OFFSET not implemented");
                        break;
                    case MIDI_META_EVENT::TEXT:
                        str = utils::chars_vector_to_string(++(e.data.begin()), e.data.end());
                        logT(std::format("Text: {}", str));
                        break;
                    case MIDI_META_EVENT::TIME_SIGNATURE:
                        logT(std::format("TIME_SIGNATURE: {:d}/{:d} - clocks {:d} - bb {:d} ", e.data[1], utils::powerOf2(e.data[2]), e.data[3], e.data[4]));
                        break;
                    default:
                        logW(std::format("MIDI_META_EVENT_TYPES_LOW not implemented/recognized: {:#02x}", type));
                        break;
                    }
                    continue; // META event processed, go on next MIDI event
                }
                case MIDI_META_EVENT_TYPES_LOW::SYS_EX0:
                    logD("SYS_EX0 META event");
                    // TODO: it should be sent as normal event?
                    m_device->sendSysEx(e);
                    continue;
                case MIDI_META_EVENT_TYPES_LOW::SYS_EX7:
                    logD("SYS_EX7 META event");
                    // TODO: it should be sent as normal event?
                    m_device->sendSysEx(e);
                    continue;
                default:
                    logW(std::format("MIDI_META_EVENT_TYPES_LOW not implemented/recognized: {:#02x}", e.type.low));
                    break;
                }
            }
                break;
            case MIDI_EVENT_TYPES_HIGH::NOTE_OFF:
            case MIDI_EVENT_TYPES_HIGH::NOTE_ON:
            case MIDI_EVENT_TYPES_HIGH::AFTERTOUCH:
            case MIDI_EVENT_TYPES_HIGH::CONTROLLER:
            case MIDI_EVENT_TYPES_HIGH::PITCH_BEND:
                /*msg[0] = e.type.val;
                msg[1] = e.data[0];
                msg[2] = e.data[1];
                msg_size = 3;*/
                //break;
            case MIDI_EVENT_TYPES_HIGH::PROGRAM_CHANGE:
            case MIDI_EVENT_TYPES_HIGH::CHANNEL_AFTERTOUCH:
                /*msg[0] = e.type.val;
                msg[1] = e.data[0];
                msg_size = 2;*/
                break;
            default:
                logW(std::format("unrecognized MIDI EVENT type high {:#02x}", e.type.high));
                break;
            }

            if (e.delta_time != 0)
            {
                cur_time += e.delta_time;
                const uint32_t delta_delay = tempo_micros * e.delta_time  + start;
                int32_t dd = delta_delay - get_start_time(); // microseconds to wait

                while (dd > DELAY_CHUNK_MICROS && !m_force_stop) {
                    // preventing longer waits before stop a song
                    utils::delayMicro(DELAY_CHUNK_MICROS);
                    dd = delta_delay - get_start_time();
                }

                if (!m_force_stop && dd > 0)
                    utils::delayMicro(dd);
                // TODO: replace with a timer (OS timer (Windows)?) that counts ticks based on midi tempo?
                start = get_start_time();
            }

            m_device->sendEvent(e);
        }

        m_device->release(this);
        m_isPlaying = false;
    }
}
