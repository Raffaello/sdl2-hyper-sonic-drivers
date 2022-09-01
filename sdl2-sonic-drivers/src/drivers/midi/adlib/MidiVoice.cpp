#include <audio/midi/types.hpp>
#include<drivers/midi/adlib/MidiVoice.hpp>
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

            constexpr int VIBRATO_THRESHOLD = 40;   /* vibrato threshold */
            constexpr int8_t HIGHEST_NOTE = 127;


            MidiVoice::MidiVoice(const uint8_t slot, const std::unique_ptr<devices::opl::OplWriter>& oplWriter) :
                _slot(slot), _oplWriter(oplWriter.get())
            {
            }

            void MidiVoice::playNote(const bool keyOn) const noexcept
            {
                _oplWriter->writeNote(_slot, _realnote, pitch, keyOn);
            }

            int MidiVoice::allocate(
                const uint8_t channel,
                const uint8_t note_, const uint8_t volume,
                const audio::opl::banks::Op2BankInstrument_t* instrument,
                const bool secondary,
                const uint8_t chan_modulation,
                const uint8_t chan_vol,
                const uint8_t chan_pitch,
                const uint8_t chan_pan,
                const uint32_t abs_time)
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

            void MidiVoice::setVolumes(const uint8_t channelVolume, const uint8_t volume) noexcept
            {
                _volume = volume;
                setRealVolume(channelVolume);
            }

            void MidiVoice::setRealVolume(const uint8_t channelVolume) noexcept
            {
                _realvolume = _calcVolume(channelVolume);
            }

            uint8_t MidiVoice::_calcVolume(const uint8_t channelVolume) const noexcept
            {
                return  std::min<uint8_t>((static_cast<uint32_t>(channelVolume) * _volume) / 127, 127);
            }
        }
    }
}
