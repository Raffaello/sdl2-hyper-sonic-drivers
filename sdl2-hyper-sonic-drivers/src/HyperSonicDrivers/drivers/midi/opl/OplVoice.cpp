#include <HyperSonicDrivers/audio/midi/types.hpp>
#include <HyperSonicDrivers/drivers/midi/opl/OplVoice.hpp>
#include <HyperSonicDrivers/hardware/opl/OPL2instrument.h>

namespace HyperSonicDrivers::drivers::midi::opl
{
    using hardware::opl::OPL2instrument_t;

    using audio::midi::MIDI_PERCUSSION_CHANNEL;
    constexpr int VIBRATO_THRESHOLD = 40;
    constexpr int8_t HIGHEST_NOTE = 127;


    OplVoice::OplVoice(const uint8_t slot, const drivers::opl::OplWriter* oplWriter) :
        _slot(slot), _oplWriter(oplWriter)
    {
    }

    bool OplVoice::noteOff(const uint8_t channel, const uint8_t note, const uint8_t sustain) noexcept
    {
        if (isChannelBusy(channel) && _note == note)
        {
            if (sustain < SUSTAIN_THRESHOLD) {
                release(false);
                return true;
            }
            else
                _sustain = true;
        }

        return false;
    }

    bool OplVoice::pitchBend(const uint8_t channel, const uint16_t bend) noexcept
    {
        const bool b = isChannelBusy(channel);
        if (b)
        {
            _pitch = static_cast<uint16_t>(_finetune + bend);
            playNote(true);
        }

        return b;
    }

    bool OplVoice::ctrl_modulationWheel(const uint8_t channel, const uint8_t value) noexcept
    {
        const bool b = isChannelBusy(channel);
        if (b)
        {
            if (value >= VIBRATO_THRESHOLD)
            {
                if (!_vibrato)
                    _oplWriter->writeModulation(_slot, _instr, true);
                _vibrato = true;

            }
            else {
                if (_vibrato)
                    _oplWriter->writeModulation(_slot, _instr, false);
                _vibrato = false;
            }
        }

        return b;
    }

    bool OplVoice::ctrl_volume(const uint8_t channel, const uint8_t value) noexcept
    {
        const bool b = isChannelBusy(channel);
        if (b)
        {
            setRealVolume(value);
            _oplWriter->writeVolume(_slot, _instr, getRealVolume());
        }

        return b;
    }

    bool OplVoice::ctrl_panPosition(const uint8_t channel, const uint8_t value) noexcept
    {
        const bool b = isChannelBusy(channel);
        if (b)
        {
            _pan = value;
            _oplWriter->writePan(_slot, _instr, value);
        }

        return b;
    }

    bool OplVoice::releaseSustain(const uint8_t channel) noexcept
    {
        const bool b = isChannelBusy(channel) && _sustain;
        if (b)
            release(false);

        return b;
    }

    void OplVoice::playNote(const bool keyOn) const noexcept
    {
        _oplWriter->writeNote(_slot, _realnote, _pitch, keyOn);
    }

    int OplVoice::allocate(
        const uint8_t channel,
        const uint8_t note, const uint8_t volume,
        const audio::opl::banks::Op2BankInstrument_t* instrument,
        const bool secondary,
        const uint8_t chan_modulation,
        const uint8_t chan_vol,
        const uint8_t chan_pitch,
        const uint8_t chan_pan
    ) noexcept
    {
        using audio::opl::banks::OP2Bank;

        int16_t note_ = note;

        _channel = channel;
        _note = note;
        _free = false;
        _secondary = secondary;
        _pan = chan_pan;

        if (chan_modulation >= VIBRATO_THRESHOLD)
            _vibrato = true;

        setVolumes(chan_vol, volume);

        if (OP2Bank::isPercussion(instrument))
            note_ = instrument->noteNum;
        else if (channel == MIDI_PERCUSSION_CHANNEL)
            note_ = 60;  // C-5

        if (secondary && OP2Bank::supportOpl3(instrument))
            _finetune = instrument->fineTune - 0x80;
        else
            _finetune = 0;

        _pitch = _finetune + chan_pitch;

        _instr = &instrument->voices[secondary ? 1 : 0];

        if ((note_ += _instr->basenote) < 0)
            while ((note_ += 12) < 0) {}
        else if (note_ > HIGHEST_NOTE)
            while ((note_ -= 12) > HIGHEST_NOTE);

        _realnote = static_cast<uint8_t>(note_);

        _oplWriter->writeInstrument(_slot, _instr);
        if (_vibrato)
            _oplWriter->writeModulation(_slot, _instr, true);
        _oplWriter->writePan(_slot, _instr, chan_pan);
        _oplWriter->writeVolume(_slot, _instr, getRealVolume());
        playNote(true);

        return _slot;
    }

    uint8_t OplVoice::release(const bool forced) noexcept
    {
        playNote(false);
        _free = true;
        if (forced)
        {
            _oplWriter->writeChannel(0x80, _slot, 0x0F, 0x0F);  // release rate - fastest
            _oplWriter->writeChannel(0x40, _slot, 0x3F, 0x3F);  // no volume
        }
        return _slot;
    }

    void OplVoice::pause() const noexcept
    {
        _oplWriter->writeVolume(_slot, _instr, 0);
        _oplWriter->writeChannel(0x60, _slot, 0, 0); // attack, decay
        _oplWriter->writeChannel(0x80, _slot,
            _instr->sust_rel_1 & 0xF0,
            _instr->sust_rel_2 & 0xF0); // sustain, release
    }

    void OplVoice::resume() const noexcept
    {
        _oplWriter->writeChannel(0x60, _slot, _instr->att_dec_1, _instr->att_dec_2);
        _oplWriter->writeChannel(0x80, _slot, _instr->sust_rel_1, _instr->sust_rel_2);
        _oplWriter->writeVolume(_slot, _instr, getRealVolume());
        _oplWriter->writePan(_slot, getInstrument(), _pan);
    }
}