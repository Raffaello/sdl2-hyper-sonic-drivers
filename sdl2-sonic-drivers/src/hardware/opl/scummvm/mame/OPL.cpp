#include <hardware/opl/scummvm/mame/OPL.hpp>


namespace hardware::opl::scummvm::mame
{
    OPL::OPL(const OplType type, const std::shared_ptr<audio::scummvm::Mixer>& mixer)
        : EmulatedOPL(type, mixer)
    {
        if (type != OplType::OPL2) {
            throw std::runtime_error("ScummVM::MAME only support OPL2");
        }
    }

    OPL::~OPL() {
        stop();
        OPLDestroy(_opl);
        _opl = nullptr;
    }

    bool OPL::init()
    {
        if (_init)
        {
            stopCallbacks();
            OPLDestroy(_opl);
        }

        _opl = makeAdLibOPL(_mixer->getOutputRate());

        _init = (_opl != nullptr);
        return _init;
    }

    void OPL::reset() {
        OPLResetChip(_opl);
    }

    void OPL::write(int a, int v) {
        OPLWrite(_opl, a, v);
    }

    uint8_t OPL::read(int a) {
        return OPLRead(_opl, a);
    }

    void OPL::writeReg(int r, int v) {
        OPLWriteReg(_opl, r, v);
    }

    bool OPL::isStereo() const
    {
        return false;
    }

    void OPL::generateSamples(int16_t* buffer, int length) {
        YM3812UpdateOne(_opl, buffer, length);
    }
}
