#pragma once
#include <hardware/opl/scummvm/EmulatedOPL.hpp>
#include <hardware/opl/scummvm/mame/mame.hpp>
#include <cstdint>

namespace hardware::opl::scummvm::mame
{
    class OPL : public EmulatedOPL
    {
    private:
        FM_OPL* _opl = nullptr;
    public:
        explicit OPL(const OplType type, const std::shared_ptr<audio::scummvm::Mixer>& mixer);
        virtual ~OPL();

        bool init() override;
        void reset() override;

        void write(int a, int v) override;
        uint8_t read(int a) override;

        void writeReg(int r, int v) override;

        bool isStereo() const override;

    protected:
        void generateSamples(int16_t* buffer, int length) override;
    };

}
