#include <array>
#include <thread>
#include <format>
#include <HyperSonicDrivers/drivers/MIDDriver.hpp>
#include <HyperSonicDrivers/utils/algorithms.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>
#include <HyperSonicDrivers/drivers/midi/opl/OplDriver.hpp>
#include <HyperSonicDrivers/devices/Opl.hpp>
#include <HyperSonicDrivers/drivers/midi/scummvm/MidiDriver_ADLIB.hpp>
#include <HyperSonicDrivers/drivers/midi/mt32/MT32Driver.hpp>

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

    MIDDriver::MIDDriver(
        const std::shared_ptr<devices::IDevice>& device,
        const audio::mixer::eChannelGroup group,
        const uint8_t volume,
        const uint8_t pan
    ) : m_device(device), m_group(group), m_volume(volume), m_pan(pan)
    {
        // TODO: move the acquire logic where the callback is set
        // NOTE/TODO: this brings up the acquire should set up the callback too?
        // it will brings to store m_device into IMidiDriver and pass it in that constructor...
        // so not sure at the moment, but i think the driver should be responsible to acquire the hardware/device
        // when they are open, and release it when they are closed
        if (!m_device->acquire(this))
        {
            utils::throwLogE<std::runtime_error>("Device is already in used by another driver or can't be init");
        }

        // The internal Midi driver will start the device and set up the callback
        if (!resetBankOP2())
        {
            utils::throwLogE<std::runtime_error>("can't reset Midi driver");
        }
    }

    MIDDriver::~MIDDriver()
    {
        MIDDriver::stop();
        m_device->release(this);
    }

    void MIDDriver::setMidi(const std::shared_ptr<audio::MIDI>& midi) noexcept
    {
        using audio::midi::MIDI_FORMAT;
        
        if (midi->format == MIDI_FORMAT::SIMULTANEOUS_TRACK)
        {
            utils::throwLogE<std::runtime_error>("Can't support MIDI format 1 (SIMULTANEOUS_TRACK), must be converted to format 0 (SINGLE_TRACK)");
            return;
        }

        m_midi = midi;
    }

    bool MIDDriver::loadBankOP2(const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank) noexcept
    {
        if (!m_device->isOpl())
            return false;

        if (op2Bank == nullptr)
        {
            utils::logE("OP2Bank is nullptr");
            return false;
        }

        m_midiDriver.reset();

        //auto opl = std::dynamic_pointer_cast<devices::Opl>(m_device)->getOpl();
        auto opl_drv = std::make_unique<drivers::midi::opl::OplDriver>(std::dynamic_pointer_cast<devices::Opl>(m_device));
        opl_drv->setOP2Bank(op2Bank);
        m_midiDriver = std::move(opl_drv);
        return open_();
    }

    bool MIDDriver::resetBankOP2() noexcept
    {
        if (m_device->isOpl())
        {
            m_midiDriver = std::make_unique<drivers::midi::scummvm::MidiDriver_ADLIB>(std::dynamic_pointer_cast<devices::Opl>(m_device));
        }
        else
        {
            // must be mt32 (TODO)
            drivers::midi::mt32::MT32Driver m;
            m_midiDriver = std::make_unique<drivers::midi::mt32::MT32Driver>();
        }

        return open_();
    }

    //void MIDDriver::play(const std::shared_ptr<audio::MIDI>& midi) noexcept
    //{
    //    using audio::midi::MIDI_FORMAT;

    //    setMidi(midi);

    //    if (midi->format != MIDI_FORMAT::SINGLE_TRACK && midi->numTracks != 1)
    //    {
    //        logC("MIDI format single track supported only");
    //        return;
    //    }

    //    // TODO: this is to set the callback frequency
    //    if (midi->division & 0x8000)
    //    {
    //        // ticks per frame
    //        int smpte = (midi->division & 0x7FFF) >> 8;
    //        int ticksPerFrame = midi->division & 0xFF;
    //        switch (smpte)
    //        {
    //        case -24:
    //        case -25:
    //        case -29:
    //        case -30:
    //            logW("SMPTE not implemented yet");
    //            break;
    //        default:
    //            logW(std::format("Division SMPTE not known = {}", smpte));
    //        }

    //        logD(std::format("Division: Ticks per frame = {}, {}", ticksPerFrame, smpte));
    //        logW("division ticks per frame not implemented yet");
    //    }
    //    else
    //    {
    //        // ticks per quarter note
    //        logD(std::format("Division: Ticks per quarter note = {}", midi->division & 0x7FFF));
    //    }

    //    stop();
    //    if (!m_device->acquire(this)) {
    //        return;
    //    }

    //    //TODO: it would be better reusing the thread...
    //    m_player = std::jthread(&MIDDriver::processTrack, this, midi->getTrack(), midi->division & 0x7FFF);
    //    m_isPlaying = true;
    //}

    void MIDDriver::play(const uint8_t track) noexcept
    {
        if (track >= m_midi->numTracks)
        {
            logW(std::format("track not available: {}", track));
            return;
        }

        // TODO: this could be to set the callback frequency?
        //       this block is doing nothing now.....
        if (m_midi->division & 0x8000)
        {
            // ticks per frame
            int smpte = (m_midi->division & 0x7FFF) >> 8;
            int ticksPerFrame = m_midi->division & 0xFF;
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
            logD(std::format("Division: Ticks per quarter note = {}", m_midi->division & 0x7FFF));
        }

        stop();
        if (!m_device->acquire(this)) {
            return;
        }

        // TODO: set up a callback instead of the thread
        m_player = std::jthread(&MIDDriver::processTrack, this, m_midi->getTrack(track), m_midi->division & 0x7FFF);
        m_isPlaying = true;
    }

    void MIDDriver::stop() noexcept
    {
        m_force_stop = true;
        m_paused = false;
        if (m_player.joinable())
            m_player.join();
        m_force_stop = false;
        m_isPlaying = false;
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

    bool MIDDriver::open_() noexcept
    {
        if (!m_midiDriver->open(m_group, m_volume, m_pan))
        {
            utils::logE("can't open midi driver");
            return false;
        }

        return true;
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
                m_midiDriver->pause();
                do
                {
                    utils::delayMillis(PAUSE_MILLIS);
                } while (m_paused);
                m_midiDriver->resume();
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
                    m_midiDriver->send(e);
                    continue;
                case MIDI_META_EVENT_TYPES_LOW::SYS_EX7:
                    logD("SYS_EX7 META event");
                    // TODO: it should be sent as normal event?
                    m_midiDriver->send(e);
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

            m_midiDriver->send(e);
        }

        m_isPlaying = false;
    }
}
