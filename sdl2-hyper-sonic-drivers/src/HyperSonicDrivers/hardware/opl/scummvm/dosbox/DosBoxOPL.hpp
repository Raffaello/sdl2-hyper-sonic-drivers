#pragma once

#include <cstdint>
#include <memory>
#include <array>
#include <HyperSonicDrivers/hardware/opl/OPL.hpp>
#include <HyperSonicDrivers/hardware/opl/Timer.hpp>
#include <HyperSonicDrivers/hardware/opl/Chip.hpp>

namespace HyperSonicDrivers::hardware::opl::scummvm::dosbox
{
    namespace dbopl {
        struct Chip;
    } // end of namespace DBOPL

    class DosBoxOPL : public OPL
    {
        void dualWrite(const uint8_t index, const uint8_t reg, uint8_t val) noexcept;
        void free() noexcept;
    public:
        DosBoxOPL(const OplType type, const std::shared_ptr<audio::IMixer>& mixer);
        ~DosBoxOPL() override;

        bool init() override;
        void reset() override;

        void write(const uint32_t port, const uint16_t val) noexcept override;
        uint8_t read(const uint32_t port) noexcept override;

        void writeReg(const uint16_t r, const uint16_t v) noexcept override;

    protected:
        void generateSamples(int16_t* buffer, const size_t length) noexcept override;

    private:
        unsigned int m_rate = 0;
        dbopl::Chip* m_emulator = nullptr;
        std::array<hardware::opl::Chip, 2> m_chip;
        hardware::opl::Chip::register_u m_reg = { 0 };
    };
}
