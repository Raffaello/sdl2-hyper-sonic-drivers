#include <HyperSonicDrivers/hardware/opl/scummvm/dosbox/DosBoxOPL.hpp>
#include <HyperSonicDrivers/hardware/opl/scummvm/dosbox/dbopl/dbopl.hpp>
#include <chrono>
#include <algorithm>
#include <cstring>
#include <cmath>

namespace HyperSonicDrivers::hardware::opl::scummvm::dosbox
{
    DosBoxOPL::DosBoxOPL(OplType type, const std::shared_ptr<audio::IMixer>& mixer)
        : EmulatedOPL(type, mixer)
    {
    }

    DosBoxOPL::~DosBoxOPL()
    {
        stop();
        free();
    }

    void DosBoxOPL::free() noexcept
    {
        delete _emulator;
        _emulator = nullptr;
    }

    bool DosBoxOPL::init()
    {
        _init = false;
        free();

        memset(&_reg, 0, sizeof(_reg));

        _emulator = new dbopl::Chip();
        if (!_emulator)
            return false;

        dbopl::InitTables();
        _rate = m_mixer->getOutputRate();
        _emulator->Setup(_rate);

        if (type == OplType::DUAL_OPL2) {
            // Setup opl3 mode in the hander
            _emulator->WriteReg(0x105, 1);
        }

        _init = true;
        return true;
    }

    void DosBoxOPL::reset() {
        init();
    }

    void DosBoxOPL::write(const uint32_t port, const uint16_t val) noexcept
    {
        const uint8_t v = static_cast<uint8_t>(val);

        if (port & 1)
        {
            switch (type)
            {
            case OplType::OPL2:
            case OplType::OPL3:
                if (!_chip[0].write(_reg.normal, v))
                    _emulator->WriteReg(_reg.normal, v);
                break;
            case OplType::DUAL_OPL2:
                // Not a 0x??8 port, then write to a specific port
                if (!(port & 0x8)) {
                    const uint8_t index = (port & 2) >> 1;
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
            // Ask the handler to write the address
            // Make sure to clip them in the right range
            switch (type)
            {
            case OplType::OPL2:
                _reg.normal = _emulator->WriteAddr(port, v) & 0xff;
                break;
            case OplType::DUAL_OPL2:
                // Not a 0x?88 port, when write to a specific side
                if (!(port & 0x8))
                {
                    uint8_t index = (port & 2) >> 1;
                    _reg.dual[index] = val & 0xff;
                }
                else
                {
                    _reg.dual[0] = val & 0xff;
                    _reg.dual[1] = val & 0xff;
                }
                break;
            case OplType::OPL3:
                _reg.normal = _emulator->WriteAddr(port, v) & 0x1ff;
                break;
            default:
                break;
            }
        }
    }

    uint8_t DosBoxOPL::read(const uint32_t port) noexcept
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

    void DosBoxOPL::writeReg(const uint16_t r, const uint16_t v) noexcept
    {
        uint16_t tempReg = 0;
        switch (type)
        {
        case OplType::OPL2:
        case OplType::DUAL_OPL2:
        case OplType::OPL3:
            // We can't use _handler->writeReg here directly, since it would miss timer changes.

            // Backup old setup register
            tempReg = _reg.normal;

            // We directly allow writing to secondary OPL3 registers by using
            // register values >= 0x100.
            if (type == OplType::OPL3 && r >= 0x100) {
                // We need to set the register we want to write to via port 0x222,
                // since we want to write to the secondary register set.
                write(0x222, r);
                // Do the real writing to the register
                write(0x223, v);
            }
            else {
                // We need to set the register we want to write to via port 0x388
                write(0x388, r);
                // Do the real writing to the register
                write(0x389, v);
            }

            // Restore the old register
            if (type == OplType::OPL3 && tempReg >= 0x100) {
                write(0x222, tempReg & ~0x100);
            }
            else {
                write(0x388, tempReg);
            }
            break;
        default:
            break;
        };
    }

    void DosBoxOPL::dualWrite(const uint8_t index, const uint8_t reg, uint8_t val) noexcept
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
        _emulator->WriteReg(fullReg, val);
    }

    void DosBoxOPL::generateSamples(int16_t* buffer, const size_t length) noexcept
    {
        // For stereo OPL cards, we divide the sample count by 2,
        // to match stereo AudioStream behavior.
        size_t length_ = length;
        //if (type != OplType::OPL2)
        if(_emulator->opl3Active)
        {
            length_ >>= 1;
        }

        constexpr unsigned int bufferLength = 512;
        std::array<int32_t, bufferLength * 2> tempBuffer;

        if (_emulator->opl3Active)
        {
            while (length_ > 0)
            {
                const unsigned int readSamples = std::min<unsigned int>(length_, bufferLength);

                _emulator->GenerateBlock3(readSamples, tempBuffer.data());

                for (unsigned int i = 0; i < (readSamples << 1); ++i)
                    buffer[i] = static_cast<int16_t>(tempBuffer[i]);

                buffer += static_cast<int16_t>(readSamples << 1);
                length_ -= readSamples;
            }
        }
        else
        {
            while (length_ > 0)
            {
                const unsigned int readSamples = std::min<unsigned int>(length_, bufferLength << 1);

                _emulator->GenerateBlock2(readSamples, tempBuffer.data());

                for (unsigned int i = 0; i < readSamples; ++i)
                    buffer[i] = static_cast<int16_t>(tempBuffer[i]);

                buffer += readSamples;
                length_ -= readSamples;
            }
        }
    }
}
