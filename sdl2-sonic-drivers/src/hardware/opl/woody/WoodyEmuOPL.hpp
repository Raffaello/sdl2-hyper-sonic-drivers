#pragma once

#include <hardware/opl/woody/OPL.hpp>
#include "OPLChip.hpp"

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
              
                void update(int16_t* buf, const int32_t samples) override;
                void write(const int reg, const int val) override;
                void init() override;
                virtual int32_t getSampleRate() const noexcept override;
                virtual bool isStereo() const override;
            private:
                bool    _stereo;
                OPLChip _opl;
            };
        }
    }
}
