#include <algorithm>
#include <format>
#include <cassert>
#include <HyperSonicDrivers/drivers/midi/opl/OplDriver.hpp>
#include <HyperSonicDrivers/hardware/opl/OplType.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>

namespace HyperSonicDrivers::drivers::midi::opl
{
    using hardware::opl::opl2_num_channels;
    using hardware::opl::opl3_num_channels;

    using audio::midi::MIDI_PERCUSSION_CHANNEL;
    using audio::midi::MIDI_EVENT_TYPES_HIGH;
    using audio::midi::TO_HIGH;
    using hardware::opl::OPL2instrument_t;
    using hardware::opl::OplType;
    using utils::logW;
    using utils::logE;
    using utils::logC;

    // TODO: allocateVoice and getFreeSlot should be merged into 1 function

    OplDriver::OplDriver(const std::shared_ptr<hardware::opl::OPL>& opl) :
        m_opl(opl), m_opl3_mode(opl->type == OplType::OPL3),
        m_oplNumChannels(m_opl3_mode ? opl3_num_channels : opl2_num_channels)
    {
        m_oplWriter = std::make_unique<drivers::opl::OplWriter>(m_opl, m_opl3_mode);

        for (uint8_t i = 0; i < audio::midi::MIDI_MAX_CHANNELS; ++i) {
            m_channels[i] = std::make_unique<OplChannel>(i);
        }

        m_voices.resize(m_oplNumChannels);
        for (uint8_t i = 0; i < m_oplNumChannels; ++i) {
            m_voices[i] = std::make_unique<OplVoice>(i, m_oplWriter.get());
            m_voicesFreeIndex.push_back(i);
        }
    }

    OplDriver::~OplDriver()
    {
        close();
    }

    bool OplDriver::open(const audio::mixer::eChannelGroup group,
        const uint8_t volume, const uint8_t pan)
    {
        if (isOpen())
            return true;

        if (!m_oplWriter->init())
        {
            logE("Can't initialize OPL Emulator chip.");
            return false;
        }

        hardware::TimerCallBack cb = std::bind_front(&OplDriver::onTimer, this);
        auto p = std::make_shared<hardware::TimerCallBack>(cb);
        m_opl->start(p, group, volume, pan);

        m_isOpen = true;
        return true;
    }

    void OplDriver::close()
    {
        if (isOpen())
        {
            m_opl->stop();
            m_isOpen = false;
        }
    }

    void OplDriver::onTimer()
    {
        // TODO: here could process midi events,
        //       enqueued in send method
        //  if queue empty do nothing
        // must keep track of the last time it was called
        // and update is internal timer with the midievent delta.

        // NOTE changing this onTimer will effect the currnet MIDDriver using a thread.
        //      but the same logic of the thread will be performed here.
    }

    void OplDriver::send(const audio::midi::MIDIEvent& e) noexcept
    {
        switch (TO_HIGH(e.type.high))
        {
        case MIDI_EVENT_TYPES_HIGH::NOTE_OFF:
            noteOff(e.type.low, e.data[0]);
            break;
        case MIDI_EVENT_TYPES_HIGH::NOTE_ON:
            noteOn(e.type.low, e.data[0], e.data[1]);
            break;
        case MIDI_EVENT_TYPES_HIGH::AFTERTOUCH:
            logW("AFTERTOUCH not supported");
            break;
        case MIDI_EVENT_TYPES_HIGH::CONTROLLER:
            controller(e.type.low, e.data[0], e.data[1]);
            break;
        case MIDI_EVENT_TYPES_HIGH::PROGRAM_CHANGE:
            programChange(e.type.low, e.data[0]);
            break;
        case MIDI_EVENT_TYPES_HIGH::CHANNEL_AFTERTOUCH:
            logW("CHANNEL_AFTERTOUCH not supported");
            break;
        case MIDI_EVENT_TYPES_HIGH::PITCH_BEND:
            pitchBend(e.type.low, static_cast<uint16_t>((e.data[0] | (e.data[1] << 7) - 0x2000) >> 6));
            break;
        case MIDI_EVENT_TYPES_HIGH::META_SYSEX:
            logW("META_SYSEX not supported");
            break;
        default:
            logW(std::format("OplDriver: Unknown send() command {:#0x}", e.type.val));
            break;
        }
    }

    void OplDriver::pause() const noexcept
    {
        for (auto it = m_voicesInUseIndex.begin(); it != m_voicesInUseIndex.end(); ++it) {
            const uint8_t i = *it;
            if (m_opl3_mode)
                m_oplWriter->writeValue(0xC0, i, m_voices[i]->getInstrument()->feedback);
            m_voices[i]->pause();
        }
    }

    void OplDriver::resume() const noexcept
    {
        for (auto it = m_voicesInUseIndex.begin(); it != m_voicesInUseIndex.end(); ++it) {
            const uint8_t i = *it;
            m_voices[i]->resume();
        }
    }

    void OplDriver::noteOff(const uint8_t chan, const uint8_t note) noexcept
    {
        const uint8_t sustain = m_channels[chan]->sustain;

        for (auto it = m_voicesInUseIndex.begin(); it != m_voicesInUseIndex.end();) {
            // TODO: this noteOff is masking the voice Release, not nice.
            if (m_voices[*it]->noteOff(chan, note, sustain)) {
                m_voicesFreeIndex.push_back(*it);
                it = m_voicesInUseIndex.erase(it);
            }
            else
                ++it;
        }
    }

    void OplDriver::noteOn(const uint8_t chan, const uint8_t note, const uint8_t vol) noexcept
    {
        using audio::opl::banks::OP2Bank;

        const bool isPercussion = chan == MIDI_PERCUSSION_CHANNEL;
        int8_t freeSlot = getFreeOplVoiceIndex(!isPercussion);

        if (freeSlot != -1)
        {
            const uint8_t instr_index = isPercussion ?
                OP2Bank::getPercussionIndex(note) :
                m_channels[chan]->getProgram();

            const auto instr = m_op2Bank->getInstrumentPtr(instr_index);

            allocateVoice(freeSlot, chan, note, vol, instr, false);

            if (m_opl3_mode && OP2Bank::supportOpl3(instr))
            {
                freeSlot = getFreeOplVoiceIndex(true);
                if (freeSlot != -1)
                    allocateVoice(freeSlot, chan, note, vol, instr, true);
            }
        }
        else
        {
            logC(std::format("NO FREE CHANNEL? midi-ch={} - playingVoices={} -- free={}", chan, m_voicesInUseIndex.size(), m_voicesFreeIndex.size()));
        }
    }

    void OplDriver::controller(const uint8_t chan, const uint8_t control, uint8_t value) const noexcept
    {
        // MIDI_EVENT_CONTROLLER_TYPES
        switch (control)
        {
        case 0:
        case 32:
            // Bank select. Not supported
            logW(std::format("bank select value {}", value));
            break;
        case 1:
            ctrl_modulationWheel(chan, value);
            //spdlog::debug("modwheel value {}", value);
            break;
        case 7:
            ctrl_volume(chan, value);
            break;
        case 10:
            // Not Available on OPL2/AdLib.
            if (m_opl3_mode)
                ctrl_panPosition(chan, value);
            break;
        case 16:
            //pitchBendFactor(value);
            logW(std::format("pitchBendFactor value {}", value));
            break;
        case 17:
            //detune(value);
            logW(std::format("detune value {}", value));
            break;
        case 18:
            //priority(value);
            logW(std::format("priority value {}", value));
            break;
        case 64:
            ctrl_sustain(chan, value);
            break;
        case 91:
            // Effects level. Not supported.
            //effectLevel(value);
            logW(std::format("effect level value {}", value));
            break;
        case 93:
            // Chorus level. Not supported.
            //chorusLevel(value);
            logW(std::format("chorus level value {}", value));
            break;
        case 119:
            // Unknown, used in Simon the Sorcerer 2
            logW(std::format("unknown value {}", value));
            break;
        case 121:
            // reset all controllers
            logW("reset all controllers value");
            //modulationWheel(0);
            //pitchBendFactor(0);
            //detune(0);
            //sustain(false);
            break;
        case 123:
            //spdlog::debug("all notes off");
            m_oplWriter->stopAll();
            break;
        default:
            logW(std::format("OplDriver: Unknown control change message {:d} {:d}", control, value));
        }
    }

    void OplDriver::programChange(const uint8_t chan, const uint8_t program) const noexcept
    {
        m_channels[chan]->programChange(program);
    }

    void OplDriver::pitchBend(const uint8_t chan, const uint16_t bend) const noexcept
    {
        //spdlog::debug("PITCH_BEND {}", bend);
        // OPLPitchWheel
        m_channels[chan]->pitch = static_cast<int8_t>(bend);

        for (auto it = m_voicesInUseIndex.begin(); it != m_voicesInUseIndex.end(); ++it)
            m_voices[*it]->pitchBend(chan, bend);
    }


    void OplDriver::ctrl_modulationWheel(const uint8_t chan, const uint8_t value) const noexcept
    {
        m_channels[chan]->modulation = value;

        for (auto it = m_voicesInUseIndex.begin(); it != m_voicesInUseIndex.end(); ++it)
            m_voices[*it]->ctrl_modulationWheel(chan, value);
    }

    void OplDriver::ctrl_volume(const uint8_t chan, const uint8_t value) const noexcept
    {
        //spdlog::debug("volume value {} -ch={}", value, chan);

        m_channels[chan]->volume = value;
        for (auto it = m_voicesInUseIndex.begin(); it != m_voicesInUseIndex.end(); ++it)
            m_voices[*it]->ctrl_volume(chan, value/*, abs_time*/);
    }

    void OplDriver::ctrl_panPosition(const uint8_t chan, uint8_t value) const noexcept
    {
        //spdlog::debug("panPosition value {}", value);

        m_channels[chan]->pan = value -= 64;
        for (auto it = m_voicesInUseIndex.begin(); it != m_voicesInUseIndex.end(); ++it)
            m_voices[*it]->ctrl_panPosition(chan, value);
    }

    void OplDriver::ctrl_sustain(const uint8_t chan, uint8_t value) const noexcept
    {
        //spdlog::debug("sustain value {}", value);
        m_channels[chan]->sustain = value;
        if (value < SUSTAIN_THRESHOLD)
            releaseSustain(chan);
    }

    void OplDriver::releaseSustain(const uint8_t channel) const noexcept
    {
        for (auto it = m_voicesInUseIndex.begin(); it != m_voicesInUseIndex.end(); ++it)
            m_voices[*it]->releaseSustain(channel);
    }

    uint8_t OplDriver::releaseVoice(const uint8_t slot, const bool forced)
    {
        assert(slot >= 0 && slot < m_oplNumChannels);

        return m_voices[slot]->release(forced);
    }

    int OplDriver::allocateVoice(const uint8_t slot, const uint8_t channel,
        const uint8_t note, const uint8_t volume,
        const audio::opl::banks::Op2BankInstrument_t* instrument,
        const bool secondary)
    {
        const OplChannel* ch = m_channels[channel].get();

        return m_voices[slot]->allocate(
            channel, note, volume, instrument, secondary,
            ch->modulation, ch->volume, ch->pitch, ch->pan
        );
    }

    int8_t OplDriver::getFreeOplVoiceIndex(const bool force)
    {
        // TODO flag = 0, 1, 2, 3 ???  (using 2 bits)
        // 0 :=> search a free channel, and if not find try to kill a 2nd voice or oldest
        // 1 :=> search a free channel only.
        // 2 :=> like 0 but won't kill the oldest channel
        // 3 :=> is like 1 as it return when bit 0 is 1
        // -----
        // used only 0 and 2, so bit 1 only:
        // it can be a bool !kill_oldest_channel

        assert(m_voicesFreeIndex.size() + m_voicesInUseIndex.size() == m_oplNumChannels);

        if (!m_voicesFreeIndex.empty()) {
            const uint8_t i = m_voicesFreeIndex.front();
            m_voicesFreeIndex.pop_front();
            m_voicesInUseIndex.push_back(i);
            return i;
        }

        for (auto it = m_voicesInUseIndex.begin(); it != m_voicesInUseIndex.end(); ++it)
        {
            if (m_voices[*it]->isSecondary()) {
                uint8_t i = releaseVoice(*it, true);
                m_voicesInUseIndex.erase(it);
                m_voicesInUseIndex.push_back(i);
                return i;
            }
        }

        if (force)
        {
            uint8_t i = releaseVoice(m_voicesInUseIndex.front(), true);
            m_voicesInUseIndex.pop_front();
            m_voicesInUseIndex.push_back(i);
            return i;
        }

        return -1;
    }
}
