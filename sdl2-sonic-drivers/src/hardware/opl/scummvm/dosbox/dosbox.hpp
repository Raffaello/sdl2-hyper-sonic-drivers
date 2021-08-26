#pragma once

#include <cstdint>
#include <hardware/opl/scummvm/EmulatedOPL.hpp>
#include <audio/scummvm/Mixer.hpp>
#include <memory>
#include <hardware/opl/scummvm/Config.hpp>

namespace hardware
{
    namespace opl
    {
        namespace scummvm
        {
            namespace dosbox
            {
                struct Timer
                {
                    double startTime;
                    double delay;
                    bool enabled, overflow, masked;
                    uint8_t counter;

                    Timer();

                    //Call update before making any further changes
                    void update(double time);

                    //On a reset make sure the start is in sync with the next cycle
                    void reset(double time);

                    void stop();

                    void start(double time, int scale);
                };

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
                    Config::OplType _type;
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
                    OPL(const std::shared_ptr<audio::scummvm::Mixer> mixer, Config::OplType type);
                    ~OPL();

                    bool init();
                    void reset();

                    void write(int a, int v);
                    uint8_t read(int a);

                    void writeReg(int r, int v);

                    bool isStereo() const { return _type != Config::OplType::OPL2; }

                protected:
                    void generateSamples(int16_t* buffer, int length);
                };
            } // End of namespace DOSBox
        }
    } // End of namespace OPL
}
