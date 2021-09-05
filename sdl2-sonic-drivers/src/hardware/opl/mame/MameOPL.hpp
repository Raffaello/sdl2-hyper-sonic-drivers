#pragma once

#include <hardware/opl/scummvm/EmulatedOPL.hpp>
#include <hardware/opl/mame/ymfm/ymfm_opl.h>

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
            public:
                MameOPL(const std::shared_ptr<audio::scummvm::Mixer> mixer);
                virtual ~MameOPL();

                bool init() override;
                void reset() override;

                void write(int a, int v) override;
                uint8_t read(int a) override;

                void writeReg(int r, int v) override;

                bool isStereo() const override;

            protected:
                void generateSamples(int16_t* buffer, int length) override;
            };
        }
    }
}
