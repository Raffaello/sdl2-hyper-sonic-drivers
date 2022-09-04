#include <drivers/midi/devices/SbPro2.hpp>

namespace drivers
{
    namespace midi
    {
        namespace devices
        {
            SbPro2::SbPro2(const std::shared_ptr<hardware::opl::OPL>& opl, const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank)
                : Opl(opl, op2Bank, true)
            {
            }
        }
    }
}
