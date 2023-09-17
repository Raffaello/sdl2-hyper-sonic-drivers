#include <HyperSonicDrivers/devices/Opl.hpp>
#include <HyperSonicDrivers/hardware/opl/OPLFactory.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>
#include <std/OplEmulatorFormatter.hpp>

namespace HyperSonicDrivers::devices
{
    Opl::Opl(
        const std::shared_ptr<audio::IMixer>& mixer,
        //const audio::mixer::eChannelGroup group,
        const hardware::opl::OplEmulator emulator,
        const hardware::opl::OplType type,
        const uint8_t volume, const uint8_t pan) //:
        //IDevice(mixer, group, volume, pan),
        //m_opl_emulator(emulator)
    {
        using hardware::opl::OPLFactory;
        using utils::logC;

        m_opl = OPLFactory::create(emulator, type, mixer);
        if (m_opl == nullptr || !m_opl->init())
        {
            logC("can't initialize opl emulator");
        }
    }
}
