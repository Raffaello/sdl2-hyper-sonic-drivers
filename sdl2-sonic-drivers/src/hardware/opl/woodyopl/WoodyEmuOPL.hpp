#pragma once

#include <hardware/opl/OPL.hpp>
#include "OPLChip.hpp"

namespace hardware
{
    namespace opl
    {
        namespace woodyopl
        {
            class WoodyEmuOPL : public OPL
            {
            public:
                WoodyEmuOPL(const int rate, const bool usestereo) noexcept;
              
                virtual void update(int16_t* buf, const int32_t samples);

                // template methods
                virtual void write(const int reg, const int val);

                virtual void init();

            private:
                bool    _stereo;
                OPLChip opl;
            };
        }
    }
}
