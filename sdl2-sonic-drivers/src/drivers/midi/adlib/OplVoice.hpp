#pragma once

#include <cstdint>
#include <hardware/opl/OPL2instrument.h>
#include <audio/opl/banks/OP2Bank.hpp>
#include <drivers/midi/devices/opl/OplWriter.hpp>

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
                OplVoice(const uint8_t slot, const std::unique_ptr<devices::opl::OplWriter>& oplWriter);
                ~OplVoice() = default;

                inline const uint8_t getSlot() const noexcept;

                inline const bool isChannel(const uint8_t channel) const noexcept;
                inline const bool isChannelBusy(const uint8_t channel) const noexcept;
                inline const bool isChannelFree(uint8_t channel) const noexcept;
                
                /*inline*/ void noteOff(const uint8_t channel, const uint8_t note, const uint8_t sustain_) noexcept;

                /*inline*/ void pitchBend(const uint8_t channel, const uint16_t bend, const uint32_t abs_time) noexcept;

                /*inline*/ void ctrl_modulationWheel(const uint8_t channel, const uint8_t value, const uint32_t abs_time) noexcept;
                /*inline*/ void ctrl_volume(const uint8_t channel, const uint8_t value, const uint32_t abs_time) noexcept;
                /*inline*/ void ctrl_panPosition(const uint8_t channel, const uint8_t value, const uint32_t abs_time) noexcept;
                
                /*inline*/ void releaseSustain(const uint8_t channel) noexcept;

            public:
                void playNote(const bool keyOn) const noexcept;
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

            public:
                inline uint8_t getRealVolume() const noexcept;

                uint8_t _channel = 0;                        // MIDI channel number
                uint8_t _note = 0;                           /* note number */
                uint8_t _realnote = 0;                       /* adjusted note number */
                int8_t  finetune = 0;                       /* frequency fine-tune */
                int16_t pitch = 0;                          /* pitch-wheel value */

                // TODO: instead a MidiChannel should be Connected to a MidiVoice, i think...
                // TODO: use pointer / share_ptr instead of copying the struct
                const hardware::opl::OPL2instrument_t* _instr = nullptr; /* current instrument */
                uint32_t time = 0;                                /* note start time */
                // Channel flags
                bool _free = true;
                bool _secondary = false;
                bool sustain = false;
                bool vibrato = false;

            private:
                const uint8_t _slot;                        /* OPL channel number */
                uint8_t _volume = 0;                        /* note volume */
                uint8_t _realvolume = 0;                     /* adjusted note volume */


                const devices::opl::OplWriter* _oplWriter;

                inline uint8_t _calcVolume(const uint8_t channelVolume) const noexcept;
            };
        }
    }
}
