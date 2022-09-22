#pragma once

#include <cstdint>
#include <hardware/opl/scummvm/EmulatedOPL.hpp>
#include <hardware/opl/Config.hpp>
#include <memory>
#include <audio/scummvm/Mixer.hpp>
#include <hardware/opl/scummvm/nuked/opl3.h>
#include <hardware/opl/Chip.hpp>

namespace hardware::opl::scummvm::nuked
{
    class NukedOPL : public EmulatedOPL
    {
    private:
        unsigned int _rate = 0;
        std::unique_ptr<opl3_chip> chip;
        hardware::opl::Chip _chip[2];
        hardware::opl::Chip::register_u _reg;

        //unsigned int address[2];

        // TODO: this is same in DOSBOX, it can be bring into the parent, and just change the last line
        void dualWrite(const uint8_t index, const uint8_t reg, uint8_t val) noexcept;
    public:
        NukedOPL(const OplType type, const std::shared_ptr<audio::scummvm::Mixer>& mixer);
        ~NukedOPL();

        bool init() override;
        void reset() override;

        // TODO this is the same as DOSBOX, it changes only the call to the emulated chip.
        void write(const int port, const int val) noexcept override; 
        // TODO this is the same as DOSBOX, here works only on OPL3
        uint8_t read(const int port) noexcept override;

        void writeReg(const int r, const int v) noexcept override;
    
    protected:
        void generateSamples(int16_t* buffer, int length) noexcept override;
    };
}
