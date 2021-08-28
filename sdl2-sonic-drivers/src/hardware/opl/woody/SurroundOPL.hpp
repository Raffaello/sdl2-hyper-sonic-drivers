


#include <cstdint>
#include <hardware/opl/scummvm/EmulatedOPL.hpp>
#include <audio/scummvm/Mixer.hpp>
#include <memory>
#include <hardware/opl/scummvm/Config.hpp>
#include <hardware/opl/woody/WoodyEmuOPL.hpp>

namespace hardware
{
    namespace opl
    {
        namespace woody
        {
            // The right-channel is increased in frequency by itself divided by this amount.
            // The right value should not noticeably change the pitch, but it should provide
            // a nice stereo harmonic effect.
            constexpr double FREQ_OFFSET = 128.0;

            // Number of FNums away from the upper/lower limit before switching to the next
            // block (octave.)  By rights it should be zero, but for some reason this seems
            // to cut it too close and the transposed OPL doesn't hit the right note all the
            // time.  Setting it higher means it will switch blocks sooner and that seems
            // to help.  Don't set it too high or it'll get stuck in an infinite loop if
            // one block is too high and the adjacent block is too low ;-)
            constexpr int NEWBLOCK_LIMIT = 32;

            // TODO: Can't work with EmulatedOPL with WoodyEmuOPL extending EmulatedOPL too.
            //       restore as it was with its own OPL interface and then eventually extend that one
            //       to EmulatedOPL.
            class SurroundOPL : public scummvm::EmulatedOPL
            {
                
            private:
                bool _use16bit;
                short bufsize;
                short* lbuf, * rbuf;
                WoodyEmuOPL* a;
                WoodyEmuOPL* b;
                uint8_t iFMReg[2][256];
                uint8_t iTweakedFMReg[2][256];
                uint8_t iCurrentTweakedBlock[2][9]; // Current value of the Block in the tweaked OPL chip
                uint8_t iCurrentFNum[2][9];         // Current value of the FNum in the tweaked OPL chip
                int8_t currChip = 0;
                scummvm::Config::OplType _type;
            public:
                SurroundOPL(const std::shared_ptr<audio::scummvm::Mixer> mixer);
                virtual ~SurroundOPL();

                bool init() override;
                void reset() override;
                void write(int a, int v) override;
                uint8_t read(int a) override;
                void writeReg(int r, int v) override;
                bool isStereo() const override;

            protected:
                void generateSamples(int16_t* buffer, int numSamples) override;
            };
        }
    }
}
