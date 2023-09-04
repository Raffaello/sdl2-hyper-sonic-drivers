#pragma once
#include <HyperSonicDrivers/hardware/opl/scummvm/EmulatedOPL.hpp>
#include <HyperSonicDrivers/hardware/opl/scummvm/mame/mame.hpp>
#include <HyperSonicDrivers/hardware/opl/Chip.hpp>
#include <cstdint>
#include <array>

namespace HyperSonicDrivers::hardware::opl::scummvm::mame
{
    class MameOPL2 : public EmulatedOPL
    {
    private:
        FM_OPL* _opl = nullptr;
        std::array<hardware::opl::Chip, 2> _chip;
        hardware::opl::Chip::register_u _reg = { 0 };

        // TODO: not used as it is OPL2
        void dualWrite(const uint8_t index, const uint8_t reg, uint8_t val) noexcept;
    public:
        explicit MameOPL2(const OplType type, const std::shared_ptr<audio::scummvm::Mixer>& mixer);
        ~MameOPL2() override;

        bool init() override;
        void reset() override;

        void write(const uint32_t port, const uint16_t val) noexcept override;
        uint8_t read(const uint32_t port) noexcept override;

        void writeReg(const uint16_t r, const uint16_t v) noexcept override;

    protected:
        void generateSamples(int16_t* buffer, int length) noexcept override;
    };

}