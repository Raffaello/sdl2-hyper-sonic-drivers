#pragma once

#include <hardware/opl/scummvm/EmulatedOPL.hpp>
#include <hardware/opl/woody/WoodyEmuOPL.hpp>

namespace hardware
{
    namespace opl
    {
        namespace woody
        {
            class WoodyOPL : public scummvm::EmulatedOPL
            {
            public:
                WoodyOPL(const std::shared_ptr<audio::scummvm::Mixer> mixer);
                virtual ~WoodyOPL();

                virtual bool init() override;
                virtual void reset() override;
                virtual void write(int a, int v) override;
                virtual uint8_t read(int a) override;
                virtual void writeReg(int r, int v) override;

                bool isStereo() const override;
            protected:
                virtual void generateSamples(int16_t* buffer, int numSamples);

            private:
                WoodyEmuOPL* _opl;

                void free();
            };
        }
    }
}
