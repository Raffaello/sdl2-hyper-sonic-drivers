#pragma once

#include <cstdint>
#include <hardware/opl/scummvm/EmulatedOPL.hpp>
#include <hardware/opl/scummvm/Config.hpp>
#include <memory>
#include <audio/scummvm/Mixer.hpp>
#include <hardware/opl/scummvm/nuked/opl3.h>

namespace hardware
{
    namespace opl
    {
        namespace scummvm
        {
            namespace nuked
            {
                class OPL : public EmulatedOPL
                {
                private:
                    Config::OplType _type;
                    unsigned int _rate;
                    std::unique_ptr<opl3_chip> chip;
                    unsigned int address[2];
                    void dualWrite(uint8_t index, uint8_t reg, uint8_t val);

                public:
                    OPL(const std::shared_ptr<audio::scummvm::Mixer> mixer, Config::OplType type);
                    ~OPL();

                    bool init() override;
                    void reset() override;

                    void write(int a, int v) override;
                    uint8_t read(int a) override;

                    void writeReg(int r, int v) override;

                    bool isStereo() const override { return true; }

                protected:
                    void generateSamples(int16_t* buffer, int length);
                };
            }
        }
    }
}
