#include <HyperSonicDrivers/devices/Adlib.hpp>
#include <HyperSonicDrivers/hardware/opl/OPLFactory.hpp>
#include <HyperSonicDrivers/hardware/opl/OplType.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>
#include <std/OplEmulatorFormatter.hpp>

namespace HyperSonicDrivers::devices
{
    Adlib::Adlib(
        const std::shared_ptr<audio::IMixer>& mixer,
        const audio::mixer::eChannelGroup group,
        const hardware::opl::OplEmulator emulator,
        const uint8_t volume, const uint8_t pan) :
        m_group(group), m_opl_emulator(emulator), m_mixer(mixer),
        m_volume(volume), m_pan(pan)
    {
        using drivers::MIDDriver;
        using drivers::westwood::ADLDriver;
        using hardware::opl::OPLFactory;
        using hardware::opl::OplType;
        using utils::logC;

        m_opl = OPLFactory::create(m_opl_emulator, OplType::OPL2, m_mixer);
        if (m_opl == nullptr || !m_opl->init())
        {
            logC(std::format("can't initialize adlib emulator type: {}", m_opl_emulator));
        }

        m_adl_drv = std::make_unique<ADLDriver>(m_opl, m_group, m_volume, m_pan);
        // TODO:
        //m_midi_adlib = std::make_unique<drivers::midi::devices::Adlib>(m_mixer,)
        //m_mid_drv = std::make_unique<MIDDriver>()
    }
}
