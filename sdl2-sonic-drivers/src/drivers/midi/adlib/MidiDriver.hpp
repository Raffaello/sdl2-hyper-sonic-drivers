#pragma once

#include <audio/midi/MIDIEvent.hpp>
#include <audio/midi/types.hpp>
#include <drivers/midi/adlib/MidiChannel.hpp>
#include <hardware/opl/OPL.hpp>
#include <memory>
#include <cstdint>
#include <files/dmx/OP2File.hpp>
#include <hardware/opl/OPL2instrument.h>

namespace drivers
{
    namespace midi
    {
        namespace adlib
        {
            /* OP2 instrument file entry */
            // TODO: This is OP2File:instrument_t related
            //typedef struct OP2instrEntry
            //{
            //    /*00*/	uint16_t                flags;      // see FL_xxx below
            //    /*02*/	uint8_t                 finetune;   // finetune value for 2-voice sounds
            //    /*03*/	uint8_t                 note;       // note # for fixed instruments
            //    /*04*/	struct OPL2instrument   instr[2];   // instruments
            ////} OP2instrEntry;

            //static_assert(sizeof(OP2instrEntry) == sizeof(files::dmx::OP2File::instrument_t));

            class MidiDriver
            {
            public:
                MidiDriver(std::shared_ptr<hardware::opl::OPL> opl, std::shared_ptr<files::dmx::OP2File> op2file);
                ~MidiDriver();

                void send(const audio::midi::MIDIEvent& e) /*const*/ noexcept;

            private:

                MidiChannel _channels[audio::midi::MIDI_MAX_CHANNELS];
                files::dmx::OP2File::instrument_t _instruments[audio::midi::MIDI_MAX_CHANNELS];
                //OPL2instrument _instruments[128];
                std::shared_ptr<files::dmx::OP2File> _op2file;

                std::shared_ptr<hardware::opl::OPL> _opl;
                
                
                void onTimer();
                
                void init() const noexcept;

                /*
                 * Stop all sounds
                 */
                void stopAll() const noexcept;
                
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
                void writeInstrument(const uint8_t channel, const hardware::opl::OPL2instrument* instr) const noexcept;

                /*
                 * Write pan (balance) data to a channel
                 */
                void writePan(const uint8_t channel, const hardware::opl::OPL2instrument* instr, const int8_t pan) const noexcept;
               
                /*
                 * Write volume data to a channel
                 */
                void writeVolume(const uint8_t channel, const hardware::opl::OPL2instrument* instr, const uint8_t volume) const noexcept;

                /*
                 * Adjust volume value (register 0x40)
                 */
                uint8_t convertVolume(const uint8_t data, const uint8_t volume) const noexcept;

                uint8_t panVolume(const uint8_t volume, int8_t pan) const noexcept;

                /*
                 * Write frequency/octave/keyon data to a channel
                 */
                void writeFreq(const uint8_t channel, const uint8_t freq, const uint8_t octave, const bool keyon) const noexcept;

                /*
                * Write a Note
                */
                void writeNote(const uint8_t channel, const uint8_t note, int pitch) const noexcept;
            };
        }
    }
}
