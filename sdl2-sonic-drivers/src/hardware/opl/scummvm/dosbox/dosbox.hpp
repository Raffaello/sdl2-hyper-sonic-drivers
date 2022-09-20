#pragma once

#include <cstdint>
#include <hardware/opl/scummvm/EmulatedOPL.hpp>
#include <audio/scummvm/Mixer.hpp>
#include <memory>
#include <hardware/opl/Config.hpp>
#include <hardware/opl/Timer.hpp>

namespace hardware
{
    namespace opl
    {
        namespace scummvm
        {
            namespace dosbox
            {
                struct Chip
                {
                    //Last selected register
                    Timer timer[2];
                    //Check for it being a write to the timer
                    bool write(uint32_t addr, uint8_t val);
                    //Read the current timer state, will use current double
                    uint8_t read();
                };

                namespace dbopl {
                    struct Chip;
                } // end of namespace DBOPL

                class OPL : public EmulatedOPL
                {
                private:
                    unsigned int _rate;

                    dbopl::Chip* _emulator;
                    Chip _chip[2];
                    union {
                        uint16_t normal;
                        uint8_t dual[2];
                    } _reg;

                    void free();
                    void dualWrite(uint8_t index, uint8_t reg, uint8_t val);
                public:
                    OPL(const OplType type, const std::shared_ptr<audio::scummvm::Mixer>& mixer);
                    virtual ~OPL();

                    bool init() override;
                    void reset() override;

                    void write(int a, int v) override;
                    uint8_t read(int a) override;

                    void writeReg(int r, int v) override;

                    bool isStereo() const override;

                protected:
                    void generateSamples(int16_t* buffer, int length) override;
                };
            } // End of namespace DOSBox
        }
    } // End of namespace OPL
}
