#include <hardware/opl/scummvm/nuked/NukedOPL3.hpp>
#include <hardware/opl/scummvm/nuked/opl3.h>
#include <cstdlib>
#include <cstring>

namespace hardware::opl::scummvm::nuked
{
    NukedOPL::NukedOPL(const OplType type, const std::shared_ptr<audio::scummvm::Mixer>& mixer)
        : EmulatedOPL(type, mixer)
    {
        chip = std::make_unique<opl3_chip>();
    }

    NukedOPL::~NukedOPL()
    {
        stop();
    }

    bool NukedOPL::init()
    {
        memset(&_reg, 0, sizeof(_reg));
        _rate = _mixer->getOutputRate();
        OPL3_Reset(chip.get(), _rate);

        if (type == OplType::DUAL_OPL2) {
            OPL3_WriteReg(chip.get(), 0x105, 0x01);
        }

        _init = true;
        return _init;
    }

    void NukedOPL::reset()
    {
        OPL3_Reset(chip.get(), _rate);
    }

    void NukedOPL::write(const uint32_t port, const uint8_t val) noexcept
    {
        if (port & 1)
        {
            switch (type)
            {
            case OplType::OPL2:
            case OplType::OPL3:
                if (!_chip[0].write(_reg.normal, val)) {
                    OPL3_WriteRegBuffered(chip.get(), _reg.normal, (uint8_t)val);
                }
                break;
            case OplType::DUAL_OPL2:
                // Not a 0x??8 port, then write to a specific port
                if (!(port & 0x8)) {
                    uint8_t index = (port & 2) >> 1;
                    dualWrite(index, _reg.dual[index], val);
                }
                else {
                    //Write to both ports
                    dualWrite(0, _reg.dual[0], val);
                    dualWrite(1, _reg.dual[1], val);
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

    void NukedOPL::writeReg(const int r, const int v) noexcept
    {
        //int tempReg = 0;
        //switch (type)
        //{
        //case OplType::OPL2:
        //case OplType::DUAL_OPL2:
        //case OplType::OPL3:
        //    // We can't use _handler->writeReg here directly, since it would miss timer changes.

        //    // Backup old setup register
        //    tempReg = _reg.normal;

        //    // We directly allow writing to secondary OPL3 registers by using
        //    // register values >= 0x100.
        //    if (type == OplType::OPL3 && r >= 0x100) {
        //        // We need to set the register we want to write to via port 0x222,
        //        // since we want to write to the secondary register set.
        //        write(0x222, r);
        //        // Do the real writing to the register
        //        write(0x223, v);
        //    }
        //    else {
        //        // We need to set the register we want to write to via port 0x388
        //        write(0x388, r);
        //        // Do the real writing to the register
        //        write(0x389, v);
        //    }

        //    // Restore the old register
        //    if (type == OplType::OPL3 && tempReg >= 0x100) {
        //        write(0x222, tempReg & ~0x100);
        //    }
        //    else {
        //        write(0x388, tempReg);
        //    }
        //    break;
        //default:
        //    break;
        //};

        OPL3_WriteRegBuffered(chip.get(), static_cast<uint16_t>(r), static_cast<uint8_t>(v));
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

        uint32_t fullReg = reg + (index ? 0x100 : 0);
        OPL3_WriteRegBuffered(chip.get(), (uint16_t)fullReg, (uint8_t)val);
    }

    uint8_t NukedOPL::read(const int port) noexcept
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

    void NukedOPL::generateSamples(int16_t* buffer, int length) noexcept
    {
        OPL3_GenerateStream(chip.get(), buffer, length / 2);
    }
}
