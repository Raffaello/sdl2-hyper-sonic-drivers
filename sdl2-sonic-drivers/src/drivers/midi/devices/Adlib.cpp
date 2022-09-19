#include <drivers/midi/devices/Adlib.hpp>

namespace drivers
{
    namespace midi
    {
        namespace devices
        {
            Adlib::Adlib(const std::shared_ptr<hardware::opl::OPL>& opl, const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank)
                : Opl(opl, op2Bank, false)
            {
            }

            Adlib::Adlib(const hardware::opl::OplEmulator emuType,
                const std::shared_ptr<audio::scummvm::Mixer>& mixer,
                const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank)
                : Opl(hardware::opl::OplType::OPL2, emuType, mixer, op2Bank)
            {
            }

            Adlib::Adlib(const std::shared_ptr<audio::scummvm::Mixer>& mixer,
                const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank)
                : Adlib(hardware::opl::OplEmulator::AUTO, mixer, op2Bank)
            {
            }
        }
    }
}
