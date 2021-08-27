#pragma once

#include <hardware/opl/scummvm/EmulatedOPL.hpp>
#include <hardware/opl/woody/OPLChip.hpp>
#include <audio/scummvm/Mixer.hpp>
#include <hardware/opl/scummvm/Config.hpp>
#include <memory>

namespace hardware
{
    namespace opl
    {
        namespace woody
        {
            class WoodyEmuOPL : public scummvm::EmulatedOPL
            {
            public:
                WoodyEmuOPL(const std::shared_ptr<audio::scummvm::Mixer> mixer, const bool stereo);
                virtual ~WoodyEmuOPL();
                
                bool init() override;
                void reset() override;
                void write(int a, int v) override;
                uint8_t read(int a) override;
                void writeReg(int r, int v) override;
                //int32_t getSampleRate() const noexcept;
                bool isStereo() const override;
            protected:
                void generateSamples(int16_t* buffer, int numSamples) override;
            private:
                bool _stereo;
                // TODO move into OPL interface
                scummvm::Config::OplType _type;
                OPLChip _opl;
            };
        }
    }
}
