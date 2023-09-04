#include <format>
#include <cassert>
#include <HyperSonicDrivers/drivers/midi/devices/Opl.hpp>
#include <HyperSonicDrivers/hardware/opl/OPLFactory.hpp>
#include <std/OplTypeFormatter.hpp>
#include <std/OplEmulatorFormatter.hpp>
#include <SDL2/SDL_log.h>

namespace HyperSonicDrivers::drivers::midi::devices
{
    Opl::Opl(const std::shared_ptr<hardware::opl::OPL>& opl, const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank)
        : Device()/*, OplDriver(opl, op2Bank, opl3_mode)*/
    {
        if (opl == nullptr) {
            const char* msg = "opl is nullptr";
            SDL_LogCritical(SDL_LOG_CATEGORY_AUDIO, msg);
            throw std::runtime_error(msg);
        }

        _oplDriver = std::make_shared<drivers::midi::opl::OplDriver>(opl, op2Bank);
    }

    Opl::Opl(const hardware::opl::OplType type,
        const hardware::opl::OplEmulator emuType,
        const std::shared_ptr<audio::scummvm::Mixer>& mixer,
        const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank)
    {
        auto opl = hardware::opl::OPLFactory::create(emuType, type, mixer);
        if (opl == nullptr || opl->type != type)
        {
            const std::string s = std::format("device Opl not supporting emu_type={}, type={}", emuType, type);
            SDL_LogCritical(SDL_LOG_CATEGORY_AUDIO, s.c_str());
            throw std::runtime_error(s);
        }
        _oplDriver = std::make_shared<drivers::midi::opl::OplDriver>(opl, op2Bank);
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
