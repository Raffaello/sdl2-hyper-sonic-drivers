#include <algorithm>
#include <array>
#include <HyperSonicDrivers/drivers/opl/OplWriter.hpp>
#include <HyperSonicDrivers/drivers/opl/tables.h>

namespace HyperSonicDrivers::drivers::opl
{
    OplWriter::OplWriter(const std::shared_ptr<hardware::opl::OPL>& opl, const bool opl3_mode) :
        _opl(opl), _opl3_mode(opl3_mode),
        _oplNumChannels(opl3_mode ? opl3_num_channels : opl2_num_channels)
    {
    }

    OplWriter::~OplWriter()
    {
        // deinit
        stopAll();
        if (_opl3_mode)
        {
            _opl->writeReg(0x105, 0x00);    // disable YMF262/OPL3 mode
            _opl->writeReg(0x104, 0x00);    // disable 4-operator mode
        }

        _opl->writeReg(0x01, 0x20); // enable Waveform Select
        _opl->writeReg(0x08, 0x00); // turn off CSW mode
        _opl->writeReg(0xBD, 0x00); // set vibrato/tremolo depth to low, set melodic mode
    }

    bool OplWriter::init() const noexcept
    {
        if (!_opl->init()) {
            return false;
        }

        if (_opl3_mode)
        {
            _opl->writeReg(0x105, 0x01);    // enable YMF262/OPL3 mode
            _opl->writeReg(0x104, 0x00);    // disable 4-operator mode
        }

        _opl->writeReg(0x01, 0x20); // enable Waveform Select
        _opl->writeReg(0x08, 0x40); // turn off CSW mode
        _opl->writeReg(0xBD, 0x00); // set vibrato/tremolo depth to low, set melodic mode

        stopAll();
        return true;
    }

    void OplWriter::stopAll() const noexcept
    {
        for (uint8_t i = 0; i < _oplNumChannels; i++)
        {
            writeChannel(0x40, i, 0x3F, 0x3F);  // turn off volume
            writeChannel(0x60, i, 0xFF, 0xFF);  // the fastest attack, decay
            writeChannel(0x80, i, 0x0F, 0x0F);  // ... and release

            writeValue(0xB0, i, 0);             // KEY-OFF
        }
    }

    void OplWriter::writeValue(const uint16_t regbase, const uint8_t channel, const uint8_t value) const noexcept
    {
        // OPL3 compatible channels
        static constexpr const std::array<uint16_t, opl3_num_channels> reg_num = {
        0x000, 0x001, 0x002, 0x003, 0x004, 0x005, 0x006, 0x007, 0x008,
        0x100, 0x101, 0x102, 0x103, 0x104, 0x105, 0x106, 0x107, 0x108 };

        _opl->writeReg(regbase + reg_num[channel], value);
    }

    void OplWriter::writeChannel(const uint16_t regbase, const uint8_t channel, const uint8_t data1, const uint8_t data2) const noexcept
    {
        // OPL3 compatible channels
        static constexpr const std::array<uint16_t, opl3_num_channels> op_num = {
        0x000, 0x001, 0x002, 0x008, 0x009, 0x00A, 0x010, 0x011, 0x012,
        0x100, 0x101, 0x102, 0x108, 0x109, 0x10A, 0x110, 0x111, 0x112 };

        const uint16_t reg = regbase + op_num[channel];
        _opl->writeReg(reg, data1);
        _opl->writeReg(reg + 3, data2);
    }

    void OplWriter::writeInstrument(const uint8_t channel, const hardware::opl::OPL2instrument_t* instr) const noexcept
    {
        writeChannel(0x40, channel, 0x3F, 0x3F);    // no volume
        writeChannel(0x20, channel, instr->trem_vibr_1, instr->trem_vibr_2);
        writeChannel(0x60, channel, instr->att_dec_1, instr->att_dec_2);
        writeChannel(0x80, channel, instr->sust_rel_1, instr->sust_rel_2);
        writeChannel(0xE0, channel, instr->wave_1, instr->wave_2);
        writeValue(0xC0, channel, instr->feedback | 0x30);
    }

    void OplWriter::writeModulation(const uint8_t slot, const hardware::opl::OPL2instrument_t* instr, const bool on) const noexcept
    {
        const uint8_t state = on ? 0x40 : 0; /* enable Frequency Vibrato */
        writeChannel(0x20, slot,
            (instr->feedback & 1) ? (instr->trem_vibr_1 | state) : instr->trem_vibr_1,
            instr->trem_vibr_2 | state);
    }

    void OplWriter::writePan(const uint8_t channel, const hardware::opl::OPL2instrument_t* instr, const int8_t pan) const noexcept
    {
        // OPL2 is mono
        // TODO: what about DUAL_OPL2 ? (use an OPL3 emulator :)
        if (!_opl3_mode)
            return;

        uint8_t bits;
        if (pan < -36) bits = 0x10;     // left
        else if (pan > 36) bits = 0x20; // right
        else bits = 0x30;               // both

        writeValue(0xC0, channel, instr->feedback | bits);
    }

    void OplWriter::writeVolume(const uint8_t channel, const hardware::opl::OPL2instrument_t* instr, const uint8_t volume) const noexcept
    {
        writeChannel(0x40, channel, ((instr->feedback & 1) ?
            _convertVolume(instr->level_1, volume) : instr->level_1) | instr->scale_1,
            _convertVolume(instr->level_2, volume) | instr->scale_2);
    }

    void OplWriter::writeNote(const uint8_t slot, const uint8_t note, int pitch, const bool keyOn) const noexcept
    {
        uint16_t freq = freqtable[note];
        uint8_t octave = octavetable[note];

        if (pitch != 0)
        {
            pitch = std::clamp(pitch, -128, 127);
            freq = static_cast<uint16_t>((static_cast<uint32_t>(freq) * pitchtable[pitch + 128]) >> 15);
            if (freq >= 1024)
            {
                freq >>= 1;
                octave++;
            }
        }
        if (octave > 7) {
            octave = 7;
        }

        _writeFreq(slot, freq, octave, keyOn);
    }

    /// <summary>
    /// 0-64 OPL volume range is kind of inverted as 0 is max volume.
    /// </summary>
    uint8_t OplWriter::_convertVolume(const uint8_t data, const uint8_t volume) noexcept
    {
#if 0
        const uint8_t vol = std::min<uint8_t>(volume, 127);
        unsigned n = 0x3F - (data & 0x3F);
        n = (n * (unsigned)volumetable[vol]) >> 7;
        return (0x3F - n) | (data & 0xC0);
#else
        return 0x3F - (((0x3F - data) *
            (unsigned)volumetable[std::min<uint8_t>(volume, 127)]) >> 7);
#endif
    }

    void OplWriter::_writeFreq(const uint8_t slot, const uint16_t freq, const uint8_t octave, const bool keyon) const noexcept
    {
        writeValue(0xA0, slot, freq & 0xFF);
        writeValue(0xB0, slot, (freq >> 8) | (octave << 2) | (static_cast<uint8_t>(keyon) << 5));
    }
}
