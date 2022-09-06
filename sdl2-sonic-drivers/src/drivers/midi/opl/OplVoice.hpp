#pragma once

#include <cstdint>
#include <hardware/opl/OPL2instrument.h>
#include <audio/opl/banks/OP2Bank.hpp>
#include <drivers/opl/OplWriter.hpp>

namespace drivers
{
    namespace midi
    {
        namespace opl
        {
            constexpr int SUSTAIN_THRESHOLD = 64;

            class OplVoice
            {
            public:
                explicit OplVoice(const uint8_t slot, const drivers::opl::OplWriter* oplWriter);
                ~OplVoice() = default;

                inline uint8_t getSlot() const noexcept { return _slot; }
                inline bool isFree() const noexcept { return _free; }
                inline bool isSecondary() const noexcept { return _secondary; }
                //inline const uint32_t getTime() const noexcept { return _time; }
                inline bool isChannel(const uint8_t channel) const noexcept { return _channel == channel; }
                inline bool usingChannel(const uint8_t channel) const noexcept { return isChannel(channel) && !_free; }
                //inline bool isChannelFree(uint8_t channel) const noexcept { return isChannel(channel) && _free; }
                
                /// <summary>
                /// It might release the note depending on sustains value
                /// </summary>
                /// <param name="channel"></param>
                /// <param name="note"></param>
                /// <param name="sustain"></param>
                /// <returns>true = voice released. false=voice sutained</returns>
                bool noteOff(const uint8_t channel, const uint8_t note, const uint8_t sustain) noexcept;
                bool pitchBend(const uint8_t channel, const uint16_t bend/*, const uint32_t abs_time*/) noexcept;
                bool ctrl_modulationWheel(const uint8_t channel, const uint8_t value/*, const uint32_t abs_time*/) noexcept;
                bool ctrl_volume(const uint8_t channel, const uint8_t value/*, const uint32_t abs_time*/) noexcept;
                bool ctrl_panPosition(const uint8_t channel, const uint8_t value/*, const uint32_t abs_time*/) const noexcept;
                bool releaseSustain(const uint8_t channel) noexcept;

                void playNote(const bool keyOn) const noexcept;
                /// <summary>
                /// This works only with OP2Bank.
                /// TODO: need to generalize OplBank instruments ...
                /// </summary>
                int allocate(const uint8_t channel,
                    const uint8_t note, const uint8_t volume,
                    const audio::opl::banks::Op2BankInstrument_t* instrument,
                    const bool secondary,
                    const uint8_t chan_modulation,
                    const uint8_t chan_vol,
                    const uint8_t chan_pitch,
                    const uint8_t chan_pan
                    //const uint32_t abs_time
                ) noexcept;
                
                uint8_t release(const bool forced) noexcept;

                inline void setVolumes(const uint8_t channelVolume, const uint8_t volume) noexcept {
                    _volume = volume;
                    setRealVolume(channelVolume);
                }
                inline void setRealVolume(const uint8_t channelVolume) noexcept { _realvolume = _calcVolume(channelVolume); }
                inline uint8_t getRealVolume() const noexcept { return _realvolume; }
                inline uint8_t getChannel() const noexcept { return _channel; }
                inline const hardware::opl::OPL2instrument_t* getInstrument() const noexcept { return _instr; }
                inline bool getVibrato() const noexcept { return _vibrato; }

                void pause() const noexcept;
                void resume() const noexcept;

            protected:
                // Methods to Mock the class, not really used except for mocking
                inline void setChannel(const uint8_t channel) noexcept { _channel = channel; }
                inline void setFree(const bool free) noexcept { _free = free; };
                inline void setInstrument(const hardware::opl::OPL2instrument_t* instr) noexcept { _instr = instr; }
                inline void setVibrato(const bool vibrato) noexcept { _vibrato = vibrato; };

            private:
                const uint8_t _slot;                        /* OPL channel number */
                
                uint8_t _volume = 0;                        /* note volume */
                uint8_t _realvolume = 0;                     /* adjusted note volume */
                uint8_t _channel = 0;                        // MIDI channel number
                uint8_t _note = 0;                           /* note number */
                uint8_t _realnote = 0;                       /* adjusted note number */
                int8_t  _finetune = 0;                       /* frequency fine-tune */
                int16_t _pitch = 0;                          /* pitch-wheel value */

                const hardware::opl::OPL2instrument_t* _instr = nullptr; /* current instrument */

                //uint32_t _time = 0;                                /* note start time */
                // Channel flags
                bool _free = true;
                bool _secondary = false;
                bool _sustain = false;
                bool _vibrato = false;

                const drivers::opl::OplWriter* _oplWriter;

                /// <summary>
                /// The volume is between 0-127 as a per MIDI specification.
                /// OPLWriter expect a MIDI volume value and converts to OPL value.
                /// OPL chips has a volume attenuation (inverted values)
                /// range from 0-64 inverted (0 is max, 64 is muted).
                /// </summary>
                inline uint8_t _calcVolume(const uint8_t channelVolume) const noexcept {
                    return  std::min<uint8_t>((static_cast<uint32_t>(channelVolume) * _volume / 127), 127);
                }
            };
        }
    }
}
