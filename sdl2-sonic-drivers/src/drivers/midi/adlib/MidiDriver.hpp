#pragma once

#include <audio/midi/MIDIEvent.hpp>
#include <audio/midi/types.hpp>
//#include <drivers/midi/adlib/MidiChannel.hpp>
#include <hardware/opl/OPL.hpp>
#include <memory>
#include <cstdint>
#include <hardware/opl/OPL2instrument.h>
#include <audio/opl/banks/OP2Bank.hpp>

namespace drivers
{
    namespace midi
    {
        namespace adlib
        {
            constexpr uint8_t OPL2_NUM_CHANNELS = 9;
            //constexpr uint8_t OPL3_NUM_CHANNELS = 18;

#define CH_SECONDARY	0x01
#define CH_SUSTAIN	0x02
#define CH_VIBRATO	0x04		/* set if modulation >= MOD_MIN */
#define CH_FREE		0x80
#define MOD_MIN		40		/* vibrato threshold */

#define FL_FIXED_PITCH	0x0001		// note has fixed pitch (see below)
//#define FL_UNKNOWN	0x0002		// ??? (used in instrument #65 only)
#define FL_DOUBLE_VOICE	0x0004		// use two voices instead of one

#define HIGHEST_NOTE 127

            /* OPL channel (voice) data */
            // TODO make a class and move to MidiChannel(rename to OPLChannel?) as OPLVoice?
            typedef struct channelEntry {
                uint8_t channel;		/* MUS channel number */
                //uint8_t musnumber;		/* MUS handle number */
                uint8_t note;			/* note number */
                uint8_t flags;			/* see CH_xxx below */
                uint8_t realnote;		/* adjusted note number */
                int8_t  finetune;		/* frequency fine-tune */
                int16_t pitch;			/* pitch-wheel value */
                uint8_t volume;			/* note volume */
                uint8_t realvolume;		/* adjusted note volume */
                hardware::opl::OPL2instrument_t* instr;	/* current instrument */
                uint32_t	time;			/* note start time */
            } channelEntry;

            /* Internal variables */
            // like a MidiChannel 
            // TODO merge with MidiChannel
            typedef struct OPLdata {
                uint8_t	channelInstr[audio::midi::MIDI_MAX_CHANNELS];		// instrument #
                uint8_t	channelVolume[audio::midi::MIDI_MAX_CHANNELS];	// volume
                uint8_t	channelLastVolume[audio::midi::MIDI_MAX_CHANNELS];	// last volume
                int8_t	channelPan[audio::midi::MIDI_MAX_CHANNELS];		// pan, 0=normal
                int8_t	channelPitch[audio::midi::MIDI_MAX_CHANNELS];		// pitch wheel, 0=normal
                uint8_t	channelSustain[audio::midi::MIDI_MAX_CHANNELS];	// sustain pedal value
                uint8_t	channelModulation[audio::midi::MIDI_MAX_CHANNELS];	// modulation pot value
            } OPLdata;

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
                channelEntry _oplChannels[OPL2_NUM_CHANNELS];

                //MidiChannel _channels[audio::midi::MIDI_MAX_CHANNELS];
                // TODO: use pointer / share_ptr instead of copying the struct
                audio::opl::banks::Op2BankInstrument_t _instruments[audio::midi::MIDI_MAX_CHANNELS];
                OPLdata _oplData;

                uint8_t _playingChannels = 0;

                void onTimer();

                void init() const noexcept;

                /*
                 * Stop all sounds
                 */
                void stopAll() const noexcept;

                uint8_t calcVolume(const uint8_t channelVolume,/* const uint8_t MUSvolume,*/ uint8_t noteVolume) const noexcept;

                void releaseSustain(const uint8_t channel);

                uint8_t releaseChannel(const uint8_t slot, const bool killed);

                int occupyChannel(const uint8_t slot, const uint8_t channel, uint8_t note, uint8_t volume, audio::opl::banks::Op2BankInstrument_t* instrument, const uint8_t secondary, const uint32_t abs_time);

                int8_t findFreeOplChannel(const uint8_t flag,  const uint32_t abs_time);

                // TOOD use a shared_ptr instead?
                audio::opl::banks::Op2BankInstrument_t* getInstrument(const uint8_t chan, const uint8_t note);

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
                void writeFreq(const uint8_t channel, const uint16_t freq, const uint8_t octave, const bool keyon) const noexcept;

                /*
                * Write a Note
                */
                void writeNote(const uint8_t channel, const uint8_t note, int pitch, const bool keyOn) const noexcept;
            };
        }
    }
}
