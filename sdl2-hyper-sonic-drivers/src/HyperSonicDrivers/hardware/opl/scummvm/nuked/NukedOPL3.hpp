#pragma once

#include <cstdint>
#include <memory>
#include <array>
#include <HyperSonicDrivers/hardware/opl/scummvm/EmulatedOPL.hpp>
#include <HyperSonicDrivers/audio/scummvm/Mixer.hpp>
#include <HyperSonicDrivers/hardware/opl/scummvm/nuked/opl3.h>
#include <HyperSonicDrivers/hardware/opl/Chip.hpp>

namespace HyperSonicDrivers::hardware::opl::scummvm::nuked
{
    class NukedOPL : public EmulatedOPL
    {
    private:
        unsigned int _rate = 0;
        std::unique_ptr<opl3_chip> chip;
        std::array<hardware::opl::Chip, 2> _chip;
        hardware::opl::Chip::register_u _reg = { 0 };

        // TODO: this is same in DOSBOX, it can be bring into the parent, and just change the last line
        void dualWrite(const uint8_t index, const uint8_t reg, uint8_t val) noexcept;
    public:
        NukedOPL(const OplType type, const std::shared_ptr<audio::scummvm::Mixer>& mixer);
        ~NukedOPL() override = default;

        bool init() override;
        void reset() override;

        // TODO this is the same as DOSBOX, it changes only the call to the emulated chip.
        void write(const uint32_t port, const uint16_t val) noexcept override;
        // TODO this is the same as DOSBOX, here works only on OPL3
        uint8_t read(const uint32_t port) noexcept override;

        void writeReg(const uint16_t r, const uint16_t v) noexcept override;
    
    protected:
        void generateSamples(int16_t* buffer, const size_t length) noexcept override;
    };
}
