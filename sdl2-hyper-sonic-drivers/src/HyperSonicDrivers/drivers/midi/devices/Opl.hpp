#pragma once

#include <memory>
#include <HyperSonicDrivers/audio/IMixer.hpp>
#include <HyperSonicDrivers/drivers/midi/Device.hpp>
#include <HyperSonicDrivers/drivers/midi/opl/OplDriver.hpp>
#include <HyperSonicDrivers/audio/opl/banks/OP2Bank.hpp>
#include <HyperSonicDrivers/hardware/opl/OPL.hpp>
#include <HyperSonicDrivers/hardware/opl/OplEmulator.hpp>

namespace HyperSonicDrivers::drivers::midi::devices
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
        // NOTE/TODO: it shouldn't use a shared_ptr for OPL emulator, but it should have ownership of the OPL.
        //            So it would be better that the Opl Device is creating its own hardware::opl emulator to use.
        // At the moment i don't see any reason why the OPL should be shared outside the "device" ...

        // TODO review the constructors and use a load bank instead..
        /** @deprecated */
        [[deprecated("use the other constructor that creates the OPL chip internally")]] explicit Opl(
            const std::shared_ptr<hardware::opl::OPL>& opl,
            const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank);

        explicit Opl(
            const hardware::opl::OplType type,
            const hardware::opl::OplEmulator emuType,
            const std::shared_ptr<audio::IMixer>& mixer,
            const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank);
        ~Opl() override = default;

    private:
        std::shared_ptr<drivers::midi::opl::OplDriver> _oplDriver;
    };
}
