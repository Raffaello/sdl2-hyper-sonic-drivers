#pragma once

#include <HyperSonicDrivers/hardware/opl/OPL.hpp>
#include <HyperSonicDrivers/hardware/opl/mame/ymfm/ymfm_opl.h>
#include <HyperSonicDrivers/hardware/opl/mame/opl3/ymf262.h>

namespace HyperSonicDrivers::hardware::opl::mame
{
    class MameOPL3 : public OPL
    {
    private:
        // OPL3
        ymfm::ymf262* _opl;
        ymfm::ymfm_interface _ymfm;
        void* _chip;
    public:
        explicit MameOPL3(const OplType type, const std::shared_ptr<audio::IMixer>& mixer);
        virtual ~MameOPL3();

        bool init() override;
        void reset() override;

        void write(const uint32_t port, const uint16_t val) noexcept override;
        uint8_t read(const uint32_t port) noexcept override;

        void writeReg(const uint16_t r, const uint16_t v) noexcept override;

    protected:
        void generateSamples(int16_t* buffer, const size_t length) noexcept override;
    };
}
