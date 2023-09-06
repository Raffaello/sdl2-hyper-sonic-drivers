#pragma once

#include <cstdint>
#include <memory>
#include <array>
#include <HyperSonicDrivers/hardware/opl/scummvm/EmulatedOPL.hpp>
#include <HyperSonicDrivers/audio/scummvm/Mixer.hpp>
#include <HyperSonicDrivers/hardware/opl/Timer.hpp>
#include <HyperSonicDrivers/hardware/opl/Chip.hpp>

namespace HyperSonicDrivers::hardware::opl::scummvm::dosbox
{
    namespace dbopl {
        struct Chip;
    } // end of namespace DBOPL

    class DosBoxOPL : public EmulatedOPL
    {
        void dualWrite(const uint8_t index, const uint8_t reg, uint8_t val) noexcept;
        void free() noexcept;
    public:
        DosBoxOPL(const OplType type, const std::shared_ptr<audio::scummvm::Mixer>& mixer);
        ~DosBoxOPL() override;

        bool init() override;
        void reset() override;

        void write(const uint32_t port, const uint16_t val) noexcept override;
        uint8_t read(const uint32_t port) noexcept override;

        void writeReg(const uint16_t r, const uint16_t v) noexcept override;

    protected:
        void generateSamples(int16_t* buffer, const size_t length) noexcept override;

    private:
        unsigned int _rate = 0;
        dbopl::Chip* _emulator = nullptr;
        std::array<hardware::opl::Chip, 2> _chip;
        hardware::opl::Chip::register_u _reg = { 0 };
    };
}
