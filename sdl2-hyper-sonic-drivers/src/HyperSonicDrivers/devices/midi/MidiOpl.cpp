#include <format>
#include <cassert>
#include <HyperSonicDrivers/devices/midi/MidiOpl.hpp>
#include <HyperSonicDrivers/hardware/opl/OPLFactory.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>
#include <std/OplTypeFormatter.hpp>
#include <std/OplEmulatorFormatter.hpp>

namespace HyperSonicDrivers::devices::midi
{
    using utils::throwLogC;

    MidiOpl::MidiOpl(
        const std::shared_ptr<hardware::opl::OPL>& opl,
        const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank,
        const audio::mixer::eChannelGroup group,
        const uint8_t volume,
        const uint8_t pan)
        : IMidiDevice()
    {
        if (opl == nullptr)
        {
            throwLogC<std::runtime_error>("opl is nullptr");
        }

        _oplDriver = std::make_shared<drivers::midi::opl::OplDriver>(opl,  group, volume, pan);
        _oplDriver->setOP2Bank(op2Bank);
    }

    MidiOpl::MidiOpl(const hardware::opl::OplType type,
        const hardware::opl::OplEmulator emuType,
        const std::shared_ptr<audio::IMixer>& mixer,
        const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank,
        const audio::mixer::eChannelGroup group,
        const uint8_t volume,
        const uint8_t pan)
    {
        auto opl = hardware::opl::OPLFactory::create(emuType, type, mixer);
        if (opl == nullptr || opl->type != type)
        {
            throwLogC<std::runtime_error>(std::format("device Opl not supporting emu_type={}, type={}", emuType, type));
        }
        _oplDriver = std::make_shared<drivers::midi::opl::OplDriver>(opl, group, volume, pan);
        _oplDriver->setOP2Bank(op2Bank);
    }

    void MidiOpl::sendEvent(const audio::midi::MIDIEvent& e) const noexcept
    {
        //this->send(e);
        _oplDriver->send(e);
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
        _oplDriver->pause();
    }

    void MidiOpl::resume() const noexcept
    {
        _oplDriver->resume();
    }
}
