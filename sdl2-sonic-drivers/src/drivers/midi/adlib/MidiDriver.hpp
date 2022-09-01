#pragma once

#include <memory>
#include <cstdint>
#include <audio/midi/MIDIEvent.hpp>
#include <audio/midi/types.hpp>
#include <audio/opl/banks/OP2Bank.hpp>
#include <drivers/midi/adlib/MidiChannel.hpp>
#include <drivers/midi/adlib/MidVoice.hpp>
#include <hardware/opl/OPL.hpp>
#include <hardware/opl/OPL2instrument.h>


namespace drivers
{
    namespace midi
    {
        namespace adlib
        {
            constexpr uint8_t OPL2_NUM_CHANNELS = 9;
            //constexpr uint8_t OPL3_NUM_CHANNELS = 18;

            /// <summary>
            /// OPL2 MidiDriver.
            /// TODO: OPL3 later
            /// 
            /// Requires instruments as input.
            /// OP2File based data bank. (It is specific to this file how to play notes and take out instruments)
            /// TODO: generalize later on...
            /// </summary>
            class MidiDriver
            {
            public:
                MidiDriver(const std::shared_ptr<hardware::opl::OPL>& opl, const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank);
                ~MidiDriver();

                void send(const audio::midi::MIDIEvent& e) /*const*/ noexcept;

            private:
                std::shared_ptr<audio::opl::banks::OP2Bank> _op2Bank;
                std::shared_ptr<hardware::opl::OPL> _opl;
                uint8_t _oplNumChannels = OPL2_NUM_CHANNELS;
                std::array<std::unique_ptr<MidiChannel>, audio::midi::MIDI_MAX_CHANNELS>  _channels;
                std::array<std::unique_ptr<MidiVoice>, OPL2_NUM_CHANNELS> _voices; // TODO shouldn't be connected to MidiChannel instead?

                uint8_t _playingVoices = 0; // OPL Channels

                void onTimer();
                void init() const noexcept;

                /*
                 * Stop all sounds
                 */
                void stopAll() const noexcept;

                uint8_t calcVolume(const uint8_t channelVolume, uint8_t noteVolume) const noexcept;

                void releaseSustain(const uint8_t channel);

                uint8_t releaseChannel(const uint8_t slot, const bool killed);

                int occupyChannel(const uint8_t slot, const uint8_t channel, uint8_t note, uint8_t volume, const audio::opl::banks::Op2BankInstrument_t* instrument, const bool secondary, const uint32_t abs_time);

                int8_t findFreeOplChannel(const uint8_t flag,  const uint32_t abs_time);

                // TOOD use a shared_ptr instead?
                //const audio::opl::banks::Op2BankInstrument_t* getInstrument(const uint8_t chan, const uint8_t note);

                /// end "midi" methods

                // start opl methods

                /*
                 * Write to an operator pair.
                 * To be used for register bases of 0x20, 0x40, 0x60, 0x80 and 0xE0.
                 */
                void writeChannel(const uint16_t regbase, const uint8_t channel, const uint8_t data1, const uint8_t data2) const noexcept;

                /*
                 * Write to channel a single value. To be used for register bases of
                 * 0xA0, 0xB0 and 0xC0.
                 */
                void writeValue(const uint16_t regbase, const uint8_t channel, const uint8_t value) const noexcept;

                /*
                 * TODO: this could be handled by the OplBank interface or soething (in OP2Bank for e.g)
                 * Write an instrument to a channel
                 *
                 * Instrument layout:
                 *
                 *   (chan+0)   (chan+3)
                 *   Operator1  Operator2  Descr.
                 *    data[0]    data[7]   reg. 0x20 - tremolo/vibrato/sustain/KSR/multi
                 *    data[1]    data[8]   reg. 0x60 - attack rate/decay rate
                 *    data[2]    data[9]   reg. 0x80 - sustain level/release rate
                 *    data[3]    data[10]  reg. 0xE0 - waveform select
                 *    data[4]    data[11]  reg. 0x40 - key scale level
                 *    data[5]    data[12]  reg. 0x40 - output level (bottom 6 bits only)
                 *          data[6]        reg. 0xC0 - feedback/AM-FM (both operators)
                 */
                void writeInstrument(const uint8_t channel, const hardware::opl::OPL2instrument_t* instr) const noexcept;

                void writeModulation(const uint8_t slot, const hardware::opl::OPL2instrument_t* instr, uint8_t state);

                /*
                 * Write pan (balance) data to a channel
                 */
                void writePan(const uint8_t channel, const hardware::opl::OPL2instrument_t* instr, const int8_t pan) const noexcept;
               
                /*
                 * Write volume data to a channel
                 */
                void writeVolume(const uint8_t channel, const hardware::opl::OPL2instrument_t* instr, const uint8_t volume) const noexcept;

                /*
                 * Adjust volume value (register 0x40)
                 */
                uint8_t convertVolume(const uint8_t data, const uint8_t volume) const noexcept;

                uint8_t panVolume(const uint8_t volume, int8_t pan) const noexcept;

                /*
                 * Write frequency/octave/keyon data to a channel
                 */
                void writeFreq(const uint8_t slot, const uint16_t freq, const uint8_t octave, const bool keyon) const noexcept;

                /*
                * Write a Note
                */
                void writeNote(const uint8_t slot, const uint8_t note, int pitch, const bool keyOn) const noexcept;
                void writeNote(const MidiVoice* voice, const bool keyOn) const noexcept;
            };
        }
    }
}
