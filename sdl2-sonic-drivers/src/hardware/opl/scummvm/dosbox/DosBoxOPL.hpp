#pragma once

#include <cstdint>
#include <hardware/opl/scummvm/EmulatedOPL.hpp>
#include <audio/scummvm/Mixer.hpp>
#include <memory>
#include <hardware/opl/Config.hpp>
#include <hardware/opl/Timer.hpp>
#include <hardware/opl/Chip.hpp>

// TODO rename the file to OPL.hpp, OPL.cpp
namespace hardware::opl::scummvm::dosbox
{
    namespace dbopl {
        struct Chip;
    } // end of namespace DBOPL

    class DosBoxOPL : public EmulatedOPL
    {
    private:
        unsigned int _rate;

        dbopl::Chip* _emulator;
        hardware::opl::Chip _chip[2];
        hardware::opl::Chip::register_u _reg;

        void free();
        void dualWrite(const uint8_t index, const uint8_t reg, uint8_t val) noexcept;
    public:
        DosBoxOPL(const OplType type, const std::shared_ptr<audio::scummvm::Mixer>& mixer);
        virtual ~DosBoxOPL();

        bool init() override;
        void reset() override;

        void write(const int port, const int val) noexcept override;
        uint8_t read(const int port) noexcept override;

        void writeReg(const int r, const int v) noexcept override;

    protected:
        void generateSamples(int16_t* buffer, int length) noexcept override;
    };
}
