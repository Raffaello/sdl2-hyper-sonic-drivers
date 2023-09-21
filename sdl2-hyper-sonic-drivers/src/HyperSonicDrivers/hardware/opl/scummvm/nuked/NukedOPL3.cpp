#include <HyperSonicDrivers/hardware/opl/scummvm/nuked/NukedOPL3.hpp>
#include <HyperSonicDrivers/hardware/opl/scummvm/nuked/opl3.h>
#include <cstdlib>
#include <cstring>

namespace HyperSonicDrivers::hardware::opl::scummvm::nuked
{
    NukedOPL::NukedOPL(const OplType type, const std::shared_ptr<audio::IMixer>& mixer)
        : OPL(mixer, type)
    {
        chip = std::make_unique<opl3_chip>();
    }

    bool NukedOPL::init()
    {
        if (m_init)
            return true;

        memset(&_reg, 0, sizeof(_reg));
        _rate = m_mixer->getOutputRate();
        OPL3_Reset(chip.get(), _rate);

        if (type == OplType::DUAL_OPL2) {
            OPL3_WriteReg(chip.get(), 0x105, 0x01);
        }

        m_init = true;
        return m_init;
    }

    void NukedOPL::reset()
    {
        OPL3_Reset(chip.get(), _rate);
    }

    void NukedOPL::write(const uint32_t port, const uint16_t val) noexcept
    {
        const uint8_t v = static_cast<uint8_t>(val);

        if (port & 1)
        {
            switch (type)
            {
            case OplType::OPL2:
            case OplType::OPL3:
                if (!_chip[0].write(_reg.normal,v)) {
                    OPL3_WriteRegBuffered(chip.get(), _reg.normal, v);
                }
                break;
            case OplType::DUAL_OPL2:
                // Not a 0x??8 port, then write to a specific port
                if (!(port & 0x8)) {
                    uint8_t index = (port & 2) >> 1;
                    dualWrite(index, _reg.dual[index], v);
                }
                else {
                    //Write to both ports
                    dualWrite(0, _reg.dual[0], v);
                    dualWrite(1, _reg.dual[1], v);
                }
                break;
            default:
                break;
            }
        }
        else {
            switch (type) {
            case OplType::OPL2:
                _reg.normal = val & 0xff;
                break;
            case OplType::DUAL_OPL2:
                // Not a 0x?88 port, when write to a specific side
                if (!(port & 0x8)) {
                    uint8_t index = (port & 2) >> 1;
                    _reg.dual[index] = val & 0xff;
                }
                else {
                    _reg.dual[0] = _reg.dual[1] = val & 0xff;
                }
                break;
            case OplType::OPL3:
                _reg.normal = (val & 0xff) | ((port << 7) & 0x100);
                break;
            default:
                break;
            }
        }
    }

    void NukedOPL::writeReg(const uint16_t r, const uint16_t v) noexcept
    {
        OPL3_WriteRegBuffered(chip.get(), r, static_cast<uint8_t>(v));
    }

    void NukedOPL::dualWrite(const uint8_t index, const uint8_t reg, uint8_t val) noexcept
    {
        // Make sure you don't use opl3 features
        // Don't allow write to disable opl3
        if (reg == 5)
            return;

        // Only allow 4 waveforms
        if (reg >= 0xE0 && reg <= 0xE8)
            val &= 3;

        // Write to the timer?
        if (_chip[index].write(reg, val))
            return;

        // Enabling panning
        if (reg >= 0xC0 && reg <= 0xC8) {
            val &= 15;
            val |= index ? 0xA0 : 0x50;
        }

        const uint16_t fullReg = reg + (index ? 0x100 : 0);
        OPL3_WriteRegBuffered(chip.get(), fullReg, val);
    }

    uint8_t NukedOPL::read(const uint32_t port) noexcept
    {
        switch (type)
        {
        case OplType::OPL2:
            if (!(port & 1))
                //Make sure the low bits are 6 on opl2
                return _chip[0].read() | 0x6;
            break;
        case OplType::OPL3:
            if (!(port & 1))
                return _chip[0].read();
            break;
        case OplType::DUAL_OPL2:
            // Only return for the lower ports
            if (port & 1)
                return 0xff;
            // Make sure the low bits are 6 on opl2
            return _chip[(port >> 1) & 1].read() | 0x6;
        default:
            break;
        }
        return 0;
    }

    void NukedOPL::generateSamples(int16_t* buffer, const size_t length) noexcept
    {
        OPL3_GenerateStream(chip.get(), buffer, length);
    }
}
