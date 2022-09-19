#pragma once

#include <memory>
#include <audio/opl/banks/OP2Bank.hpp>
#include <drivers/midi/devices/Opl.hpp>
#include <hardware/opl/OPL.hpp>

namespace drivers
{
    namespace midi
    {
        namespace devices
        {
            class Adlib : public Opl
            {
            public:
                Adlib(const std::shared_ptr<hardware::opl::OPL>& opl, const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank);
                Adlib(const hardware::opl::OplEmulator emuType,
                    const std::shared_ptr<audio::scummvm::Mixer>& mixer,
                    const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank);
                Adlib(const std::shared_ptr<audio::scummvm::Mixer>& mixer,
                    const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank);
                virtual ~Adlib() = default;

                //void loadBankOP2();
            };
        }
    }
}
