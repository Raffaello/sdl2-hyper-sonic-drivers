#pragma once

#include <cstdint>
#include <memory>
#include <hardware/opl/OPL.hpp>
#include <hardware/opl/OPL2instrument.h>

namespace HyperSonicDrivers::drivers::opl
{
    constexpr uint8_t OPL2_NUM_CHANNELS = 9;
    constexpr uint8_t OPL3_NUM_CHANNELS = 18;

    class OplWriter
    {
    public:
        OplWriter() = delete;
        OplWriter(OplWriter&) = delete;
        OplWriter(OplWriter&&) = delete;
        OplWriter& operator=(const OplWriter&) = delete;
        explicit OplWriter(const std::shared_ptr<hardware::opl::OPL>& opl, const bool opl3_mode);
        ~OplWriter();

        bool init() const noexcept;

        /// <summary>
        /// Stop all sounds
        /// </summary>
        void stopAll() const noexcept;

        /*
         * Write to channel a single value. To be used for register bases of
         * 0xA0, 0xB0 and 0xC0.
         */
        void writeValue(const uint16_t regbase, const uint8_t channel, const uint8_t value) const noexcept;

        /*
         * Write to an operator pair.
         * To be used for register bases of 0x20, 0x40, 0x60, 0x80 and 0xE0.
         */
        void writeChannel(const uint16_t regbase, const uint8_t channel, const uint8_t data1, const uint8_t data2) const noexcept;

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

        void writeModulation(const uint8_t slot, const hardware::opl::OPL2instrument_t* instr, const bool on) const noexcept;

        /*
         * Write pan (balance) data to a channel
         */
        void writePan(const uint8_t channel, const hardware::opl::OPL2instrument_t* instr, const int8_t pan) const noexcept;

        /*
         * Write volume data to a channel
         */
        void writeVolume(const uint8_t channel, const hardware::opl::OPL2instrument_t* instr, const uint8_t volume) const noexcept;

        /*
         * Write a Note
         */
        void writeNote(const uint8_t slot, const uint8_t note, int pitch, const bool keyOn) const noexcept;

    private:
        /*
         * Adjust volume value (register 0x40)
         */
        static uint8_t _convertVolume(const uint8_t data, const uint8_t volume) noexcept;

        /*
         * Write frequency/octave/keyon data to a channel
         */
        void _writeFreq(const uint8_t slot, const uint16_t freq, const uint8_t octave, const bool keyon) const noexcept;

        const bool _opl3_mode;
        const uint8_t _oplNumChannels;
        const std::shared_ptr<hardware::opl::OPL> _opl;
    };
}
