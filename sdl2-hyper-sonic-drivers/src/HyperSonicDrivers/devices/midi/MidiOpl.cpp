#include <format>
#include <cassert>
#include <HyperSonicDrivers/devices/midi/MidiOpl.hpp>
#include <HyperSonicDrivers/hardware/opl/OPLFactory.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>
#include <std/OplTypeFormatter.hpp>
#include <std/OplEmulatorFormatter.hpp>
#include <HyperSonicDrivers/drivers/midi/scummvm/MidiDriver_ADLIB.hpp>

namespace HyperSonicDrivers::devices::midi
{
    using utils::throwLogC;

    MidiOpl::MidiOpl(const hardware::opl::OplType type,
        const hardware::opl::OplEmulator emuType,
        const std::shared_ptr<audio::IMixer>& mixer,
        const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank,
        const audio::mixer::eChannelGroup group,
        const uint8_t volume,
        const uint8_t pan)
    {
        m_opl = hardware::opl::OPLFactory::create(emuType, type, mixer);
        if (m_opl == nullptr || m_opl->type != type)
        {
            throwLogC<std::runtime_error>(std::format("device Opl not supporting emu_type={}, type={}", emuType, type));
        }
        if (op2Bank == nullptr)
        {
            utils::throwLogE<std::invalid_argument>("OP2Bank is nullptr");
        }

        auto opl_drv = std::make_unique<drivers::midi::opl::OplDriver>(m_opl);
        opl_drv->setOP2Bank(op2Bank);
        m_midiDriver = std::move(opl_drv);
        if (!m_midiDriver->open(group, volume, pan))
        {
            throwLogC<std::runtime_error>("can't open OplDriver");
        }
    }

    MidiOpl::MidiOpl(
        const hardware::opl::OplType type,
        const hardware::opl::OplEmulator emuType,
        const std::shared_ptr<audio::IMixer>& mixer,
        const audio::mixer::eChannelGroup group,
        const uint8_t volume,
        const uint8_t pan)
    {
        using namespace hardware::opl;

        m_opl = OPLFactory::create(emuType, type, mixer);
        if (m_opl == nullptr || m_opl->type != type)
        {
            throwLogC<std::runtime_error>(std::format("device Opl not supporting emu_type={}, type={}", emuType, type));
        }

        m_midiDriver = std::make_unique<drivers::midi::scummvm::MidiDriver_ADLIB>(m_opl, m_opl->type != OplType::OPL2);
        m_midiDriver->open(group, volume, pan);

    }

    void MidiOpl::sendEvent(const audio::midi::MIDIEvent& e) const noexcept
    {
        //this->send(e);
        m_midiDriver->send(e);
    }

    void MidiOpl::sendMessage(const uint8_t msg[], const uint8_t size) const noexcept
    {
        assert(size >= 2 && size <= 3);
        audio::midi::MIDIEvent e;

        e.type.val = msg[0];
        e.data.push_back(msg[1]);
        if (size == 3) {
            e.data.push_back(msg[2]);
        }

        sendEvent(e);
    }

    void MidiOpl::sendSysEx(const audio::midi::MIDIEvent& e) const noexcept
    {
        // TODO
        //_adlib->send(e);
    }

    void MidiOpl::pause() const noexcept
    {
        m_midiDriver->pause();
    }

    void MidiOpl::resume() const noexcept
    {
        m_midiDriver->resume();
    }
}
