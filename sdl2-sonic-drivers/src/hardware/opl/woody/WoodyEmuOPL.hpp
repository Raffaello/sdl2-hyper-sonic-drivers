#pragma once

#include <hardware/opl/woody/OPL.hpp>
#include <hardware/opl/woody/OPLChip.hpp>

namespace hardware::opl::woody
{
    class WoodyEmuOPL : public OPL
    {
    public:
        WoodyEmuOPL(const int rate, const bool usestereo) noexcept;

        void update(int16_t* buf, const int32_t samples) override;
        void write(const uint32_t reg, const uint8_t val) override;
        void init() override;
        int32_t getSampleRate() const noexcept override;
        bool isStereo() const override;
    private:
        bool    _stereo;
        OPLChip _opl;
    };
}
