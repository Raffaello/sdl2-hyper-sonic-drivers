#include <HyperSonicDrivers/hardware/opl/scummvm/dosbox/DosBoxOPL.hpp>
#include <HyperSonicDrivers/hardware/opl/scummvm/dosbox/dbopl/dbopl.hpp>
#include <chrono>
#include <algorithm>
#include <cstring>
#include <cmath>
#include <bit>

namespace HyperSonicDrivers::hardware::opl::scummvm::dosbox
{
    DosBoxOPL::DosBoxOPL(OplType type, const std::shared_ptr<audio::IMixer>& mixer)
        : OPL(mixer, type)
    {
    }

    DosBoxOPL::~DosBoxOPL()
    {
        stop();
        free();
    }

    void DosBoxOPL::free() noexcept
    {
        m_emulator.reset();
    }

    bool DosBoxOPL::init()
    {
        m_init = false;
        free();

        memset(&m_reg, 0, sizeof(m_reg));

        m_emulator = std::make_unique<dbopl::Chip>();
        if (!m_emulator)
            return false;

        dbopl::InitTables();
        m_rate = m_mixer->freq;
        m_emulator->Setup(m_rate);

        if (type == OplType::DUAL_OPL2) {
            m_emulator->WriteReg(0x105, 1);
        }

        m_init = true;
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
                if (!m_chip[0].write(m_reg.normal, v))
                    m_emulator->WriteReg(m_reg.normal, v);
                break;
            case OplType::DUAL_OPL2:
                // Not a 0x??8 port, then write to a specific port
                if (!(port & 0x8)) {
                    const uint8_t index = (port & 2) >> 1;
                    dualWrite(index, m_reg.dual[index], v);
                }
                else {
                    //Write to both ports
                    dualWrite(0, m_reg.dual[0], v);
                    dualWrite(1, m_reg.dual[1], v);
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
                m_reg.normal = m_emulator->WriteAddr(port, v) & 0xff;
                break;
            case OplType::DUAL_OPL2:
                // Not a 0x?88 port, when write to a specific side
                if (!(port & 0x8))
                {
                    uint8_t index = (port & 2) >> 1;
                    m_reg.dual[index] = val & 0xff;
                }
                else
                {
                    // it looks very verbose to me....
                    m_reg.dual[0] = std::bit_cast<uint8_t>(static_cast<uint8_t>(val & 0xff));
                    m_reg.dual[1] = std::bit_cast<uint8_t>(static_cast<uint8_t>(val & 0xff));
                }
                break;
            case OplType::OPL3:
                m_reg.normal = m_emulator->WriteAddr(port, v) & 0x1ff;
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
                return m_chip[0].read() | 0x6;
            break;
        case OplType::OPL3:
            if (!(port & 1))
                return m_chip[0].read();
            break;
        case OplType::DUAL_OPL2:
            // Only return for the lower ports
            if (port & 1)
                return 0xff;
            // Make sure the low bits are 6 on opl2
            return m_chip[(port >> 1) & 1].read() | 0x6;
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
            tempReg = m_reg.normal;

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
        if (m_chip[index].write(reg, val))
            return;

        // Enabling panning
        if (reg >= 0xC0 && reg <= 0xC8) {
            val &= 15;
            val |= index ? 0xA0 : 0x50;
        }

        uint32_t fullReg = reg + (index ? 0x100 : 0);
        m_emulator->WriteReg(fullReg, val);
    }

    void DosBoxOPL::generateSamples(int16_t* buffer, const size_t length) noexcept
    {
        // For stereo OPL cards, we divide the sample count by 2,
        // to match stereo AudioStream behavior.
        size_t length_ = length;

        constexpr unsigned int bufferLength = 512;
        std::array<int32_t, bufferLength * 2> tempBuffer;

        if(isStereo())
        {
            if (m_emulator->opl3Active) // DUAL_OPL2 or OPL3 in OPL3 mode
            {
                while (length_ > 0)
                {
                    const unsigned int readSamples = std::min<unsigned int>(length_, bufferLength);
                    const unsigned int readSamples2 = (readSamples << 1);
                    m_emulator->GenerateBlock3(readSamples, tempBuffer.data());
                    for (unsigned int i = 0; i < readSamples2; ++i)
                        buffer[i] = static_cast<int16_t>(tempBuffer[i]);

                    buffer += static_cast<int16_t>(readSamples2);
                    length_ -= readSamples;
                }
            }
            else // OPL3 in OPL2 compatibility mode
            {
                while (length_ > 0)
                {
                    const unsigned int readSamples = std::min<unsigned int>(length_, bufferLength);
                    const unsigned int readSamples2 = (readSamples << 1);
                    m_emulator->GenerateBlock2(readSamples, tempBuffer.data());
                    for (unsigned int i = 0, j = 0; i < readSamples; ++i, j += 2)
                        buffer[j] = buffer[j + 1] = static_cast<int16_t>(tempBuffer[i]);


                    buffer += static_cast<int16_t>(readSamples2);
                    length_ -= readSamples;
                }
            }
        }
        else // OPL2
        {
            while (length_ > 0)
            {
                const unsigned int readSamples = std::min<unsigned int>(length_, bufferLength << 1);
                m_emulator->GenerateBlock2(readSamples, tempBuffer.data());
                for (unsigned int i = 0; i < readSamples; ++i)
                    buffer[i] = static_cast<int16_t>(tempBuffer[i]);

                buffer += readSamples;
                length_ -= readSamples;
            }
        }
    }
}
