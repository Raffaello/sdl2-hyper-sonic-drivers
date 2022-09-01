#include <audio/midi/types.hpp>
#include<drivers/midi/adlib/OplVoice.hpp>
#include <hardware/opl/OPL2instrument.h>

namespace drivers
{
    namespace midi
    {
        namespace adlib
        {
            using hardware::opl::OPL2instrument_t;

            using audio::opl::banks::OP2BANK_INSTRUMENT_FLAG_FIXED_PITCH;
            using audio::opl::banks::OP2BANK_INSTRUMENT_FLAG_DOUBLE_VOICE;

            using audio::midi::MIDI_PERCUSSION_CHANNEL;

            ;
            constexpr int VIBRATO_THRESHOLD = 40;   /* vibrato threshold */
            constexpr int8_t HIGHEST_NOTE = 127;


            OplVoice::OplVoice(const uint8_t slot, const std::unique_ptr<devices::opl::OplWriter>& oplWriter) :
                _slot(slot), _oplWriter(oplWriter.get())
            {
            }

            /*inline*/ const uint8_t OplVoice::getSlot() const noexcept
            {
                return _slot;
            }

            /*inline*/ const bool OplVoice::isChannel(const uint8_t channel) const noexcept
            {
                return _channel == channel;
            }

            /*inline*/ const bool OplVoice::isChannelBusy(const uint8_t channel) const noexcept
            {
                return isChannel(channel) && !_free;
            }

            /*inline*/ const bool OplVoice::isChannelFree(uint8_t channel) const noexcept
            {
                return isChannel(channel) && _free;
            }

            /*inline*/ void OplVoice::noteOff(const uint8_t channel, const uint8_t note, const uint8_t sustain_) noexcept
            {
                if (isChannelBusy(channel) && _note == note)
                {
                    if (sustain_ < SUSTAIN_THRESHOLD)
                        release(0);
                    else
                        sustain = true;
                }
            }

            /*inline*/ void OplVoice::pitchBend(const uint8_t channel, const uint16_t bend, const uint32_t abs_time) noexcept
            {
                if (isChannelBusy(channel))
                {
                    time = abs_time;
                    pitch = finetune + bend;
                    playNote(true);
                }
            }

            /*inline*/ void OplVoice::ctrl_modulationWheel(const uint8_t channel, const uint8_t value, const uint32_t abs_time) noexcept
            {
                if (isChannelBusy(channel))
                {
                    time = abs_time;
                    if (value >= VIBRATO_THRESHOLD)
                    {
                        if (!vibrato)
                            _oplWriter->writeModulation(_slot, _instr, 1);
                        vibrato = true;

                    }
                    else {
                        if (vibrato)
                            _oplWriter->writeModulation(_slot, _instr, 0);
                        vibrato = false;

                    }
                }
            }

            /*inline*/ void OplVoice::ctrl_volume(const uint8_t channel, const uint8_t value, const uint32_t abs_time) noexcept
            {
                if (isChannelBusy(channel))
                {
                    time = abs_time;
                    setRealVolume(value);
                    _oplWriter->writeVolume(_slot, _instr, getRealVolume());
                }
            }

            /*inline*/ void OplVoice::ctrl_panPosition(const uint8_t channel, const uint8_t value, const uint32_t abs_time) noexcept
            {
                if (isChannelBusy(channel))
                {
                    time = abs_time;
                    _oplWriter->writePan(_slot, _instr, value);
                }
            }

            /*inline*/ void OplVoice::releaseSustain(const uint8_t channel) noexcept
            {
                if (isChannelBusy(channel) && sustain) {
                    release(false);
                }
            }

            void OplVoice::playNote(const bool keyOn) const noexcept
            {
                _oplWriter->writeNote(_slot, _realnote, pitch, keyOn);
            }

            int OplVoice::allocate(
                const uint8_t channel,
                const uint8_t note_, const uint8_t volume,
                const audio::opl::banks::Op2BankInstrument_t* instrument,
                const bool secondary,
                const uint8_t chan_modulation,
                const uint8_t chan_vol,
                const uint8_t chan_pitch,
                const uint8_t chan_pan,
                const uint32_t abs_time) noexcept
            {
                const OPL2instrument_t* instr;
                uint16_t note = note_;

                _channel = channel;
                _note = note_;
                _free = false;
                _secondary = secondary;
                
                if (chan_modulation >= VIBRATO_THRESHOLD)
                    vibrato = true;
                
                time = abs_time;
                setVolumes(chan_vol, volume);
                
                if (instrument->flags & OP2BANK_INSTRUMENT_FLAG_FIXED_PITCH)
                    note = instrument->noteNum;
                else if (channel == MIDI_PERCUSSION_CHANNEL)
                    note = 60;  // C-5
                
                // TODO: this is OPL3
                if (secondary && (instrument->flags & OP2BANK_INSTRUMENT_FLAG_DOUBLE_VOICE))
                    finetune = instrument->fineTune - 0x80;
                else
                    finetune = 0;
                
                pitch = finetune + pitch;
                
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
                if (vibrato)
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
