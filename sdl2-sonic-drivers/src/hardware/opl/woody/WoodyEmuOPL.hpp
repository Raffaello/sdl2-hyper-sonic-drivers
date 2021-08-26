#pragma once

#include <hardware/opl/woody/OPL.hpp>
#include <hardware/opl/woody/OPLChip.hpp>

namespace hardware
{
    namespace opl
    {
        namespace woody
        {
            class WoodyEmuOPL : public OPL
            {
            public:
                WoodyEmuOPL(const int rate, const bool usestereo) noexcept;
              
                virtual void update(int16_t* buf, const int32_t samples);
                virtual void write(const int reg, const int val);
                //virtual void init();
                virtual int32_t getSampleRate() const noexcept;
            private:
                bool    _stereo;
                OPLChip _opl;
            };
        }
    }
}
