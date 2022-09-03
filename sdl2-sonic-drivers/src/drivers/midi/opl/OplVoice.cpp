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

           
            constexpr int VIBRATO_THRESHOLD = 40;   /* vibrato threshold */
            constexpr int8_t HIGHEST_NOTE = 127;


            OplVoice::OplVoice(const uint8_t slot, const std::unique_ptr<drivers::opl::OplWriter>& oplWriter) :
                _slot(slot), _oplWriter(oplWriter.get())
            {
            }

            bool OplVoice::noteOff(const uint8_t channel, const uint8_t note, const uint8_t sustain) noexcept
            {
                const bool b = isChannelBusy(channel) && _note == note;
                if (b)
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
                        if (!_vibrato)
                            _oplWriter->writeModulation(_slot, _instr, 1);
                        _vibrato = true;

                    }
                    else {
                        if (_vibrato)
                            _oplWriter->writeModulation(_slot, _instr, 0);
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
                
                // TODO: this is OPL3
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
                    _oplWriter->writeModulation(_slot, _instr, 1);
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

            void OplVoice::setVolumes(const uint8_t channelVolume, const uint8_t volume) noexcept
            {
                _volume = volume;
                setRealVolume(channelVolume);
            }

            void OplVoice::setRealVolume(const uint8_t channelVolume) noexcept
            {
                _realvolume = _calcVolume(channelVolume);
            }

            /*inline*/ uint8_t OplVoice::getRealVolume() const noexcept
            {
                return _realvolume;
            }

            uint8_t OplVoice::_calcVolume(const uint8_t channelVolume) const noexcept
            {
                return  std::min<uint8_t>((static_cast<uint32_t>(channelVolume) * _volume) / 127, 127);
            }
        }
    }
}