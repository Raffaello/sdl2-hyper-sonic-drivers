#include <hardware/opl/woody/WoodyEmuOPL.hpp>

namespace hardware::opl::woody
{
    WoodyEmuOPL::WoodyEmuOPL(const int rate, const bool usestereo) noexcept
        : OPL(), _stereo(usestereo), _opl(rate)
    {
    }

    void WoodyEmuOPL::update(int16_t* buf, const int32_t samples)
    {
        if (_stereo) {
            _opl.adlib_getsample(buf, samples * 2);
        }
        else {
            _opl.adlib_getsample(buf, samples);
        }
    }

    // template methods
    void WoodyEmuOPL::write(const uint32_t reg, const uint8_t val)
    {
        _opl.index = reg;
        _opl.adlib_write(_opl.index, val, 0);
    }

    void WoodyEmuOPL::init()
    {
    }

    int32_t WoodyEmuOPL::getSampleRate() const noexcept
    {
        return _opl.getSampleRate();
    }
    bool WoodyEmuOPL::isStereo() const
    {
        return _stereo;
    }
}
