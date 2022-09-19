#include <drivers/midi/devices/Opl.hpp>
#include <cassert>
#include <hardware/opl/Config.hpp>

namespace drivers::midi::devices
{
    Opl::Opl(const std::shared_ptr<hardware::opl::OPL>& opl, const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank, const bool opl3_mode)
        : Device()/*, OplDriver(opl, op2Bank, opl3_mode)*/
    {
        _oplDriver = std::make_shared<drivers::midi::opl::OplDriver>(opl, op2Bank, opl3_mode);
    }

    Opl::Opl(const hardware::opl::OplType type,
        const hardware::opl::OplEmulator emuType,
        const std::shared_ptr<audio::scummvm::Mixer>& mixer,
        const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank)
    {
        auto opl = hardware::opl::Config::create(emuType, type, mixer);
        _oplDriver = std::make_shared<drivers::midi::opl::OplDriver>(opl, op2Bank, type != hardware::opl::OplType::OPL2);
    }

    void Opl::sendEvent(const audio::midi::MIDIEvent& e) const noexcept
    {
        //this->send(e);
        _oplDriver->send(e);
    }

    void Opl::sendMessage(const uint8_t msg[], const uint8_t size) const noexcept
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

    void Opl::sendSysEx(const audio::midi::MIDIEvent& e) const noexcept
    {
        // TODO
        //_adlib->send(e);
    }

    void Opl::pause() const noexcept
    {
        _oplDriver->pause();

    }

    void Opl::resume() const noexcept
    {
        _oplDriver->resume();
    }
}
