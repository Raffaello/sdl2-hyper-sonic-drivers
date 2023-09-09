#include <HyperSonicDrivers/hardware/opl/scummvm/mame/MameOPL2.hpp>
#include <cstring>


namespace HyperSonicDrivers::hardware::opl::scummvm::mame
{
    MameOPL2::MameOPL2(const OplType type, const std::shared_ptr<audio::IMixer>& mixer)
        : EmulatedOPL(type, mixer)
    {
        if (type != OplType::OPL2)
        {
            throw std::invalid_argument("MameOPL2 only support OPL2");
        }
    }

    MameOPL2::~MameOPL2()
    {
        stop();
        if(_opl != nullptr)
            OPLDestroy(_opl);
        _opl = nullptr;
    }

    bool MameOPL2::init()
    {
        if (_init)
            return true;

        _opl = makeAdLibOPL(m_mixer->getOutputRate());
        memset(&_reg, 0, sizeof(_reg));
        _init = (_opl != nullptr);

        return _init;
    }

    void MameOPL2::reset()
    {
        OPLResetChip(_opl);
    }

    void MameOPL2::write(const uint32_t port, const uint16_t val) noexcept
    {
        if (port & 1)
        {
            _chip[0].write(_reg.normal, static_cast<uint8_t>(val));
            OPLWrite(_opl, port, val);
        }
        else
        {
            // Ask the handler to write the address
            // Make sure to clip them in the right range
            OPLWrite(_opl, port, val);
            _reg.normal = val & 0xff;
        }
    }

    uint8_t MameOPL2::read(const uint32_t port) noexcept
    {
        // TODO looks like mame has its own timer, but must be set it up
        if (!(port & 1))
            //Make sure the low bits are 6 on opl2
            return _chip[0].read() | 0x6;
        return 0;
        //return OPLRead(_opl, port);
    }

    void MameOPL2::writeReg(const uint16_t r, const uint16_t v) noexcept
    {
       OPLWriteReg(_opl, r, v);
    }

    void MameOPL2::generateSamples(int16_t* buffer, const size_t length) noexcept
    {
        YM3812UpdateOne(_opl, buffer, length);
    }
}
