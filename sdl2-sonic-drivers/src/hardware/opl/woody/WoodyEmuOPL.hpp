#pragma once

#include <hardware/opl/woody/OPL.hpp>
#include <hardware/opl/woody/OPLChip.hpp>

namespace hardware::opl::woody
{
    class WoodyEmuOPL : public OPL
    {
    public:
        explicit WoodyEmuOPL(const int rate) noexcept;
        ~WoodyEmuOPL() override = default;

        void update(int16_t* buf, const int32_t samples) override;
        void write(const uint32_t reg, const uint8_t val) override;
        uint8_t read(const uint32_t port) noexcept override;
        void writeReg(const uint16_t r, const uint16_t v) noexcept override;
        void init() override;
    private:
        OPLChip _opl;
    };
}
