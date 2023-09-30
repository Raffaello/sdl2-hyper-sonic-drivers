#include <HyperSonicDrivers/audio/midi/types.hpp>
#include <HyperSonicDrivers/drivers/midi/opl/OplVoice.hpp>
#include <HyperSonicDrivers/hardware/opl/OPL2instrument.h>
#include <HyperSonicDrivers/utils/ILogger.hpp>
#include <HyperSonicDrivers/drivers/midi/IMidiChannel.hpp>

namespace HyperSonicDrivers::drivers::midi::opl
{
    using audio::midi::MIDI_PERCUSSION_CHANNEL;
    using hardware::opl::OPL2instrument_t;

    constexpr int VIBRATO_THRESHOLD = 40;
    constexpr int8_t HIGHEST_NOTE = 127;

    OplVoice::OplVoice(const uint8_t slot, const drivers::opl::OplWriter* oplWriter) :
        m_slot(slot), m_oplWriter(oplWriter)
    {
    }

    bool OplVoice::noteOff(const uint8_t note, const uint8_t sustain) noexcept
    {
        if(!isFree() && m_note == note)
        {
            if (sustain < opl_sustain_threshold)
            {
                release(false);
                return true;
            }
            else
                m_sustain = true;
        }

        return false;
    }

    bool OplVoice::pitchBend(const uint16_t bend) noexcept
    {
        const bool b = !isFree();
        if (b)
        {
            m_pitch_factor = static_cast<uint16_t>(m_finetune + bend);
            playNote(true);
        }

        return b;
    }

    bool OplVoice::ctrl_modulationWheel(const uint8_t value) noexcept
    {
        const bool b = !isFree();
        if (b)
        {
            if (value >= VIBRATO_THRESHOLD)
            {
                if (!m_vibrato)
                    m_oplWriter->writeModulation(m_slot, m_instr, true);
                m_vibrato = true;

            }
            else {
                if (m_vibrato)
                    m_oplWriter->writeModulation(m_slot, m_instr, false);
                m_vibrato = false;
            }
        }

        return b;
    }

    bool OplVoice::ctrl_volume(const uint8_t value) noexcept
    {
        const bool b = !isFree();
        if (b)
        {
            setVolumes(value);
            m_oplWriter->writeVolume(m_slot, m_instr, m_real_volume);
        }

        return b;
    }

    bool OplVoice::ctrl_panPosition(const uint8_t value) noexcept
    {
        const bool b = !isFree();
        if (b)
        {
            m_channel->pan = value;
            m_oplWriter->writePan(m_slot, m_instr, value);
        }

        return b;
    }

    bool OplVoice::releaseSustain() noexcept
    {
        const bool b = !isFree() && m_sustain;
        if (b)
            release(false);

        return b;
    }

    void OplVoice::playNote(const bool keyOn) const noexcept
    {
        m_oplWriter->writeNote(m_slot, m_real_note, m_pitch_factor, keyOn);
    }

    int OplVoice::allocate(
        IMidiChannel* channel,
        const uint8_t note, const uint8_t volume,
        const audio::opl::banks::Op2BankInstrument_t* instrument,
        const bool secondary
    ) noexcept
    {
        using audio::opl::banks::OP2Bank;

        int16_t note_ = note;

        m_channel = channel;
        m_note = note;
        m_free = false;
        m_secondary = secondary;

        if (m_channel->modulation >= VIBRATO_THRESHOLD)
            m_vibrato = true;

        setVolumes(volume);

        if (OP2Bank::isPercussion(instrument))
            note_ = instrument->noteNum;
        else if (channel->isPercussion)
            note_ = 60;  // C-5

        if (secondary && OP2Bank::supportOpl3(instrument))
            m_finetune = instrument->fineTune - 0x80;
        else
            m_finetune = 0;

        m_pitch_factor = m_finetune + m_channel->pitch;

        setInstrument(&instrument->voices[secondary ? 1 : 0]);

        if ((note_ += m_instr->basenote) < 0)
            while ((note_ += 12) < 0) {}
        else if (note_ > HIGHEST_NOTE)
            while ((note_ -= 12) > HIGHEST_NOTE);

        m_real_note = static_cast<uint8_t>(note_);

        m_oplWriter->writeInstrument(m_slot, m_instr);
        if (m_vibrato)
            m_oplWriter->writeModulation(m_slot, m_instr, true);
        m_oplWriter->writePan(m_slot, m_instr, m_channel->pan);
        m_oplWriter->writeVolume(m_slot, m_instr, m_real_volume);
        playNote(true);

        return m_slot;
    }

    uint8_t OplVoice::release(const bool forced) noexcept
    {
        playNote(false);
        m_free = true;
        if (forced)
        {
            m_oplWriter->writeChannel(0x80, m_slot, 0x0F, 0x0F);  // release rate - fastest
            m_oplWriter->writeChannel(0x40, m_slot, 0x3F, 0x3F);  // no volume
        }
        return m_slot;
    }

    void OplVoice::pause() const noexcept
    {
        m_oplWriter->writeVolume(m_slot, m_instr, 0);
        m_oplWriter->writeChannel(0x60, m_slot, 0, 0); // attack, decay
        m_oplWriter->writeChannel(0x80, m_slot,
            m_instr->sust_rel_1 & 0xF0,
            m_instr->sust_rel_2 & 0xF0); // sustain, release
    }

    void OplVoice::resume() const noexcept
    {
        m_oplWriter->writeChannel(0x60, m_slot, m_instr->att_dec_1, m_instr->att_dec_2);
        m_oplWriter->writeChannel(0x80, m_slot, m_instr->sust_rel_1, m_instr->sust_rel_2);
        m_oplWriter->writeVolume(m_slot, m_instr, m_real_volume);
        m_oplWriter->writePan(m_slot, getInstrument(), m_channel->pan);
    }

    void OplVoice::setInstrument(const hardware::opl::OPL2instrument_t* instr) noexcept
    {
        if (instr == nullptr)
            utils::throwLogC<std::runtime_error>("OPL2instrument_t is null");

        m_instr = instr;
    }
}
