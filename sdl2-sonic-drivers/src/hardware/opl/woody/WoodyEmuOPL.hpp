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
                WoodyEmuOPL(const std::shared_ptr<audio::scummvm::Mixer> mixer);
              
                virtual void write(int a, int v);
                virtual bool init();
                virtual void reset();
                virtual uint8_t read(int a);
                virtual void writeReg(int r, int v);
                virtual int32_t getSampleRate() const noexcept;
            protected:
                virtual void generateSamples(int16_t* buffer, int numSamples);
            private:
                bool    _stereo;
                scummvm::Config::OplType _type;
                OPLChip _opl;
            };
        }
    }
}
