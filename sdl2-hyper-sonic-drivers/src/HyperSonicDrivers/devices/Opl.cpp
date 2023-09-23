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
        IDevice(mixer, true)
    {
        using hardware::opl::OPLFactory;
        using utils::logC;

        m_opl = OPLFactory::create(emulator, type, mixer);
        m_hardware = m_opl.get();
    }

    // TODO: move it to MIDDriver
    //       MIDDriver has to manage OplDriver and AdlibDriver etc..
    /*bool Opl::loadBankOP2(
        const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank,
        const audio::mixer::eChannelGroup group,
        const uint8_t volume,
        const uint8_t pan) noexcept
    {
        if (op2Bank == nullptr)
        {
            utils::logE("OP2Bank is nullptr");
            return false;
        }

        auto opl_drv = std::make_unique<drivers::midi::opl::OplDriver>(m_opl);
        opl_drv->setOP2Bank(op2Bank);
        m_midiDriver = std::move(opl_drv);

        if (!m_midiDriver->open(group, volume, pan))
        {
            utils::logE("can't open OplDriver");
            return false;
        }

        return true;
    }*/

    bool Opl::init() noexcept
    {
        if (isInit())
            return true;

        if (m_opl == nullptr || !m_opl->init())
        {
            utils::logE("can't initialize opl emulator");
            return false;
        }

        //m_opl->start(nullptr);

        m_init = true;
        return true;
    }

    bool Opl::shutdown() noexcept
    {
        if (m_opl != nullptr)
            m_opl->stop();

        return true;
    }

    //void Opl::sendEvent(const audio::midi::MIDIEvent& e) const noexcept
    //{
    //    m_midiDriver->send(e);
    //}

    //void Opl::sendMessage(const uint8_t msg[], const uint8_t size) const noexcept
    //{
    //    assert(size >= 2 && size <= 3);
    //    audio::midi::MIDIEvent e;

    //    e.type.val = msg[0];
    //    e.data.push_back(msg[1]);
    //    if (size == 3) {
    //        e.data.push_back(msg[2]);
    //    }

    //    sendEvent(e);
    //}

    //void Opl::sendSysEx(const audio::midi::MIDIEvent& e) const noexcept
    //{
    //    // TODO
    //    //_adlib->send(e);
    //}

    //void Opl::pause() const noexcept
    //{
    //    m_midiDriver->pause();
    //}

    //void Opl::resume() const noexcept
    //{
    //    m_midiDriver->resume();
    //}
}
