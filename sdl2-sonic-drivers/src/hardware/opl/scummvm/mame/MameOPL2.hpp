#pragma once
#include <hardware/opl/scummvm/EmulatedOPL.hpp>
#include <hardware/opl/scummvm/mame/mame.hpp>
#include <hardware/opl/Chip.hpp>
#include <cstdint>

namespace hardware::opl::scummvm::mame
{
    class MameOPL2 : public EmulatedOPL
    {
    private:
        FM_OPL* _opl = nullptr;
        hardware::opl::Chip _chip[2];
        hardware::opl::Chip::register_u _reg;

        // TODO: not used as it is OPL2
        void dualWrite(const uint8_t index, const uint8_t reg, uint8_t val) noexcept;
    public:
        explicit MameOPL2(const OplType type, const std::shared_ptr<audio::scummvm::Mixer>& mixer);
        virtual ~MameOPL2();

        bool init() override;
        void reset() override;

        void write(const int port, const int val) noexcept override;
        uint8_t read(const int port) noexcept override;

        void writeReg(const int r, const int v) noexcept override;

    protected:
        void generateSamples(int16_t* buffer, int length) noexcept override;
    };

}
