#pragma once

#include <memory>
#include <drivers/midi/Device.hpp>
#include <drivers/midi/opl/OplDriver.hpp>
#include <audio/opl/banks/OP2Bank.hpp>
#include <hardware/opl/OPL.hpp>
#include <hardware/opl/OplEmulator.hpp>
#include <audio/scummvm/Mixer.hpp>

namespace drivers::midi::devices
{
    class Opl : public Device /*, protected drivers::midi::opl::OplDriver*/
    {
    public:
        void sendEvent(const audio::midi::MIDIEvent& e) const noexcept override;
        void sendMessage(const uint8_t msg[], const uint8_t size) const noexcept override;
        void sendSysEx(const audio::midi::MIDIEvent& e) const noexcept override;
        virtual void pause() const noexcept override;
        virtual void resume() const noexcept override;

        //void loadBankOP2();
    protected:
        // TODO review the constructor and use a load bank instead..
        Opl(const std::shared_ptr<hardware::opl::OPL>& opl, const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank, const bool opl3_mode);
        Opl(const hardware::opl::OplType type,
            const hardware::opl::OplEmulator emuType,
            const std::shared_ptr<audio::scummvm::Mixer>& mixer,
            const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank);
        virtual ~Opl() = default;

    private:
        std::shared_ptr<drivers::midi::opl::OplDriver> _oplDriver;
    };
}
