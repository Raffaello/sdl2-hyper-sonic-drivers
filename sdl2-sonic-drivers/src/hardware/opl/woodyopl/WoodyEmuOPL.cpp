#include "WoodyEmuOPL.hpp"

namespace hardware
{
    namespace opl
    {
        namespace woodyopl
        {
            WoodyEmuOPL::WoodyEmuOPL(const int rate, const bool usestereo) noexcept
                : _stereo(usestereo)
            {
                opl.adlib_init(rate);
                _chip = ChipType::OPL3;
            };

            void WoodyEmuOPL::update(int16_t* buf, const int32_t samples)
            {
                //      if(use16bit) samples *= 2;
                if (_stereo) {
                    opl.adlib_getsample(buf, samples * 2);
                } else {
                    opl.adlib_getsample(buf, samples);
                }
            }

            // template methods
            void WoodyEmuOPL::write(const int reg, const int val)
            {
                if (_currentChip != 0) {
                    return;
                }

                opl.index = reg;
                opl.adlib_write(opl.index, val, 0);
            }

            void WoodyEmuOPL::init()
            {
            }
        }
    }
}
