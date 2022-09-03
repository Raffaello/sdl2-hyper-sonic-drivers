#include <audio/midi/types.hpp>
#include<drivers/midi/opl/OplVoice.hpp>
#include <hardware/opl/OPL2instrument.h>

namespace drivers
{
    namespace midi
    {
        namespace opl
        {
            using hardware::opl::OPL2instrument_t;

            using audio::opl::banks::OP2BANK_INSTRUMENT_FLAG_FIXED_PITCH;
            using audio::opl::banks::OP2BANK_INSTRUMENT_FLAG_DOUBLE_VOICE;
            using audio::midi::MIDI_PERCUSSION_CHANNEL;
            constexpr int VIBRATO_THRESHOLD = 40;
            constexpr int8_t HIGHEST_NOTE = 127;


            OplVoice::OplVoice(const uint8_t slot, const std::unique_ptr<drivers::opl::OplWriter>& oplWriter) :
                _slot(slot), _oplWriter(oplWriter.get())
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

            bool OplVoice::pitchBend(const uint8_t channel, const uint16_t bend/*, const uint32_t abs_time*/) noexcept
            {
                const bool b = isChannelBusy(channel);
                if (b)
                {
                    //_time = abs_time;
                    _pitch = _finetune + bend;
                    playNote(true);
                }

                return b;
            }

            bool OplVoice::ctrl_modulationWheel(const uint8_t channel, const uint8_t value/*, const uint32_t abs_time*/) noexcept
            {
                const bool b = isChannelBusy(channel);
                if (b)
                {
                    //_time = abs_time;
                    if (value >= VIBRATO_THRESHOLD)
                    {
                        _vibrato != _vibrato;
                        _oplWriter->writeModulation(_slot, _instr, _vibrato);

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

            bool OplVoice::ctrl_volume(const uint8_t channel, const uint8_t value/*, const uint32_t abs_time*/) noexcept
            {
                const bool b = isChannelBusy(channel);
                if (b)
                {
                    //_time = abs_time;
                    setRealVolume(value);
                    _oplWriter->writeVolume(_slot, _instr, getRealVolume());
                }

                return b;
            }

            /*inline*/ bool OplVoice::ctrl_panPosition(const uint8_t channel, const uint8_t value/*, const uint32_t abs_time*/) noexcept
            {
                const bool b = isChannelBusy(channel);
                if (b)
                {
                    //_time = abs_time;
                    _oplWriter->writePan(_slot, _instr, value);
                }

                return b;
            }

            /*inline*/ bool OplVoice::releaseSustain(const uint8_t channel) noexcept
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
                const uint8_t note_, const uint8_t volume,
                const audio::opl::banks::Op2BankInstrument_t* instrument,
                const bool secondary,
                const uint8_t chan_modulation,
                const uint8_t chan_vol,
                const uint8_t chan_pitch,
                const uint8_t chan_pan
                //const uint32_t abs_time
            ) noexcept
            {
                const OPL2instrument_t* instr;
                int16_t note = note_;

                _channel = channel;
                _note = note_;
                _free = false;
                _secondary = secondary;
                
                if (chan_modulation >= VIBRATO_THRESHOLD)
                    _vibrato = true;
                
                // = abs_time;
                setVolumes(chan_vol, volume);
                
                if (instrument->flags & OP2BANK_INSTRUMENT_FLAG_FIXED_PITCH)
                    note = instrument->noteNum;
                else if (channel == MIDI_PERCUSSION_CHANNEL)
                    note = 60;  // C-5
                
                if (secondary && (instrument->flags & OP2BANK_INSTRUMENT_FLAG_DOUBLE_VOICE))
                    _finetune = instrument->fineTune - 0x80;
                else
                    _finetune = 0;
                
                _pitch = _finetune + chan_pitch;
                
                if (secondary)
                    instr = &instrument->voices[1];
                else
                    instr = &instrument->voices[0];

                _instr = instr;

                if ((note += instr->basenote) < 0)
                    while ((note += 12) < 0);
                else if (note > HIGHEST_NOTE)
                    while ((note -= 12) > HIGHEST_NOTE);
                
                _realnote = note;

                _oplWriter->writeInstrument(_slot, _instr);
                if (_vibrato)
                    _oplWriter->writeModulation(_slot, _instr, true);
                _oplWriter->writePan(_slot, instr, chan_pan);
                _oplWriter->writeVolume(_slot, instr, getRealVolume());
                playNote(true);

                return _slot;
            }

            uint8_t OplVoice::release(const bool killed) noexcept
            {
                playNote(false);
                _free = true;
                if (killed)
                {
                    _oplWriter->writeChannel(0x80, _slot, 0x0F, 0x0F);  // release rate - fastest
                    _oplWriter->writeChannel(0x40, _slot, 0x3F, 0x3F);  // no volume
                }
                return _slot;
            }
        }
    }
}
