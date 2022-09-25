#include <hardware/opl/woody/WoodyEmuOPL.hpp>

namespace hardware::opl::woody
{
    WoodyEmuOPL::WoodyEmuOPL(const int rate) noexcept
        : OPL(), _opl(rate)
    {
    }

    void WoodyEmuOPL::update(int16_t* buf, const int32_t samples)
    {
        _opl.adlib_getsample(buf, samples);
    }

    // template methods
    void WoodyEmuOPL::write(const uint32_t reg, const uint8_t val)
    {
        _opl.index = reg;
        _opl.adlib_write(_opl.index, val, 0);
    }

    uint8_t WoodyEmuOPL::read(const uint32_t port) noexcept
    {
        if (!(port & 1))
            //Make sure the low bits are 6 on opl2
            return _opl.status | 0x6;
        return 0;
    }

    void WoodyEmuOPL::writeReg(const uint16_t r, const uint16_t v) noexcept
    {
        // Backup old setup register
        const uint8_t tempReg = _opl.index;
        // We need to set the register we want to write to via port 0x388
        write(0x388, r);
        // Do the real writing to the register
        write(0x389, v);
        // Restore the old register
        write(0x388, tempReg);
    }

    void WoodyEmuOPL::init()
    {
    }
}
