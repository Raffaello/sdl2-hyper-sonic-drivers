#pragma once

#include <hardware/opl/scummvm/EmulatedOPL.hpp>
#include <hardware/opl/mame/ymfm/ymfm_opl.h>
#include <hardware/opl/mame/opl3/ymf262.h>

namespace hardware
{
    namespace opl
    {
        namespace mame
        {
            class MameOPL : public scummvm::EmulatedOPL
            {
            private:
                // OPL3
                ymfm::ymf262* _opl;
                ymfm::ymfm_interface _ymfm;
                void* _chip;
            public:
                explicit MameOPL(const OplType type, const std::shared_ptr<audio::scummvm::Mixer>& mixer);
                virtual ~MameOPL();

                bool init() override;
                void reset() override;

                void write(const uint32_t port, const uint16_t val) noexcept override;
                uint8_t read(const uint32_t port) noexcept override;

                void writeReg(const uint16_t r, const uint16_t v) noexcept override;

            protected:
                void generateSamples(int16_t* buffer, int length) noexcept override;
            };
        }
    }
}
