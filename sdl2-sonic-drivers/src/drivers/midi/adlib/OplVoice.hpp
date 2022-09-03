#pragma once

#include <cstdint>
#include <hardware/opl/OPL2instrument.h>
#include <audio/opl/banks/OP2Bank.hpp>
#include <drivers/opl/OplWriter.hpp>

namespace drivers
{
    namespace midi
    {
        namespace adlib
        {
            constexpr int SUSTAIN_THRESHOLD = 64;

            class OplVoice
            {
            public:
                explicit OplVoice(const uint8_t slot, const std::unique_ptr<opl::OplWriter>& oplWriter);
                ~OplVoice() = default;

                inline const uint8_t getSlot() const noexcept { return _slot; }
                inline const bool isFree() const noexcept { return _free; }
                inline const bool isSecondary() const noexcept { return _secondary; }
                inline const uint32_t getTime() const noexcept { return _time; }

                inline const bool isChannel(const uint8_t channel) const noexcept;
                inline const bool isChannelBusy(const uint8_t channel) const noexcept;
                inline const bool isChannelFree(uint8_t channel) const noexcept;
                
                bool noteOff(const uint8_t channel, const uint8_t note, const uint8_t sustain) noexcept;
                bool pitchBend(const uint8_t channel, const uint16_t bend, const uint32_t abs_time) noexcept;
                bool ctrl_modulationWheel(const uint8_t channel, const uint8_t value, const uint32_t abs_time) noexcept;
                bool ctrl_volume(const uint8_t channel, const uint8_t value, const uint32_t abs_time) noexcept;
                bool ctrl_panPosition(const uint8_t channel, const uint8_t value, const uint32_t abs_time) noexcept;
                bool releaseSustain(const uint8_t channel) noexcept;

                void playNote(const bool keyOn) const noexcept;
                /// <summary>
                /// This works only with OP2Bank.
                /// TODO: need to generalize OplBank instruments ...
                /// </summary>
                int allocate(const uint8_t channel,
                    const uint8_t note_, const uint8_t volume,
                    const audio::opl::banks::Op2BankInstrument_t* instrument,
                    const bool secondary,
                    const uint8_t chan_modulation,
                    const uint8_t chan_vol,
                    const uint8_t chan_pitch,
                    const uint8_t chan_pan,
                    const uint32_t abs_time) noexcept;

                uint8_t release(const bool killed) noexcept;

                void setVolumes(const uint8_t channelVolume, const uint8_t volume) noexcept;
                void setRealVolume(const uint8_t channelVolume) noexcept;

                inline uint8_t getRealVolume() const noexcept;

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

                uint32_t _time = 0;                                /* note start time */
                // Channel flags
                bool _free = true;
                bool _secondary = false;
                bool _sustain = false;
                bool _vibrato = false;

                const opl::OplWriter* _oplWriter;

                inline uint8_t _calcVolume(const uint8_t channelVolume) const noexcept;
            };
        }
    }
}
