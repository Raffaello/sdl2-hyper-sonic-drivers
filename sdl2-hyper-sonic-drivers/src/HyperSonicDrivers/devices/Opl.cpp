#include <HyperSonicDrivers/devices/Opl.hpp>
#include <HyperSonicDrivers/drivers/midi/opl/OplDriver.hpp>
#include <HyperSonicDrivers/drivers/midi/scummvm/MidiDriver_ADLIB.hpp>
#include <HyperSonicDrivers/hardware/opl/OPLFactory.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>
#include <std/OplEmulatorFormatter.hpp>
#include <cassert>

namespace HyperSonicDrivers::devices
{
    using utils::throwLogC;

    Opl::Opl(
        const std::shared_ptr<audio::IMixer>& mixer,
        const hardware::opl::OplEmulator emulator,
        const hardware::opl::OplType type,
        const uint8_t volume, const uint8_t pan) :
        IDevice(mixer, eDeviceType::Opl),
        m_emulator(emulator)
    {
        using hardware::opl::OPLFactory;
        using utils::logC;

        m_opl = OPLFactory::create(emulator, type, mixer);
        m_hardware = m_opl.get();
    }

    bool Opl::init() noexcept
    {
        // TODO can be put in the parent class using IHardware*
        if (isInit())
            return true;

        if (m_opl == nullptr || !m_opl->init())
        {
            utils::logE("can't initialize opl emulator");
            return false;
        }

        m_init = true;
        return true;
    }

    bool Opl::shutdown() noexcept
    {
        // TODO: can be put in the parent calss using iHardware*
        if (m_opl != nullptr)
            m_opl->stop();

        return true;
    }
}
