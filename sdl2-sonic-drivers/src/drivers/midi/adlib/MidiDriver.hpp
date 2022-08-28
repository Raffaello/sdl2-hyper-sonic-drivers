#include <memory>
#include <hardware/opl/OPL.hpp>
#include <audio/midi/MIDIEvent.hpp>
#include <stdint.h>

namespace drivers
{
    namespace midi
    {
        namespace adlib
        {
            /* OPL2 instrument */
            // TODO: This is OP2File::instrument_t related
            typedef struct OPL2instrument
            {
                /*00*/  uint8_t     trem_vibr_1;    /* OP 1: tremolo/vibrato/sustain/KSR/multi */
                /*01*/  uint8_t     att_dec_1;      /* OP 1: attack rate/decay rate */
                /*02*/  uint8_t     sust_rel_1;     /* OP 1: sustain level/release rate */
                /*03*/  uint8_t     wave_1;         /* OP 1: waveform select */
                /*04*/  uint8_t     scale_1;        /* OP 1: key scale level */
                /*05*/  uint8_t     level_1;        /* OP 1: output level */
                /*06*/  uint8_t     feedback;       /* feedback/AM-FM (both operators) */
                /*07*/  uint8_t     trem_vibr_2;    /* OP 2: tremolo/vibrato/sustain/KSR/multi */
                /*08*/  uint8_t     att_dec_2;      /* OP 2: attack rate/decay rate */
                /*09*/  uint8_t     sust_rel_2;     /* OP 2: sustain level/release rate */
                /*0A*/  uint8_t     wave_2;         /* OP 2: waveform select */
                /*0B*/  uint8_t     scale_2;        /* OP 2: key scale level */
                /*0C*/  uint8_t     level_2;        /* OP 2: output level */
                /*0D*/  uint8_t     unused;
                /*0E*/  uint16_t    basenote;       /* base note offset */
            } OPL2instrument;

            /* OP2 instrument file entry */
            // TODO: This is OP2File:instrument_t related
            typedef struct OP2instrEntry
            {
                /*00*/	uint16_t                flags;      // see FL_xxx below
                /*02*/	uint8_t                 finetune;   // finetune value for 2-voice sounds
                /*03*/	uint8_t                 note;       // note # for fixed instruments
                /*04*/	struct OPL2instrument   instr[2];   // instruments
            } OP2instrEntry;


            class MidiDriver
            {
            public:
                MidiDriver(std::shared_ptr<hardware::opl::OPL> opl);
                ~MidiDriver();

                void send(const audio::midi::MIDIEvent& e) const noexcept;

            private:
                std::shared_ptr<hardware::opl::OPL> _opl;
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
                 *   Operator1  Operator2  Descr.
                 *    data[0]    data[7]   reg. 0x20 - tremolo/vibrato/sustain/KSR/multi
                 *    data[1]    data[8]   reg. 0x60 - attack rate/decay rate
                 *    data[2]    data[9]   reg. 0x80 - sustain level/release rate
                 *    data[3]    data[10]  reg. 0xE0 - waveform select
                 *    data[4]    data[11]  reg. 0x40 - key scale level
                 *    data[5]    data[12]  reg. 0x40 - output level (bottom 6 bits only)
                 *          data[6]        reg. 0xC0 - feedback/AM-FM (both operators)
                 */
                void writeInstrument(const uint8_t channel, const OPL2instrument* instr) const noexcept;

                /*
                 * Write pan (balance) data to a channel
                 */
                void writePan(const uint8_t channel, const OPL2instrument* instr, const int8_t pan) const noexcept;
               
                /*
                 * Write volume data to a channel
                 */
                void writeVolume(const uint8_t channel, const OPL2instrument* instr, const uint8_t volume) const noexcept;

                /*
                 * Adjust volume value (register 0x40)
                 */
                uint8_t convertVolume(const uint8_t data, const uint8_t volume) const noexcept;

                uint8_t panVolume(const uint8_t volume, int8_t pan) const noexcept;

                /*
                 * Write frequency/octave/keyon data to a channel
                 */
                void writeFreq(const uint8_t channel, const uint8_t freq, const uint8_t octave, uint8_t keyon) const noexcept;


                /*
                * Write a Note
                */
                void MidiDriver::writeNote(const uint8_t channel, const uint8_t note, int pitch, const uint8_t keyOn) const noexcept;

            };
        }
    }
}
