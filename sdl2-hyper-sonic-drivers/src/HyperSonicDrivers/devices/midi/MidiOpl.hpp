#pragma once

#include <memory>
#include <HyperSonicDrivers/audio/IMixer.hpp>
#include <HyperSonicDrivers/devices/IMidiDevice.hpp>
#include <HyperSonicDrivers/drivers/midi/opl/OplDriver.hpp>
#include <HyperSonicDrivers/audio/opl/banks/OP2Bank.hpp>
#include <HyperSonicDrivers/hardware/opl/OPL.hpp>
#include <HyperSonicDrivers/hardware/opl/OplEmulator.hpp>

namespace HyperSonicDrivers::devices::midi
{
    class MidiOpl : public IMidiDevice
    {
    public:
        void sendEvent(const audio::midi::MIDIEvent& e) const noexcept override;
        void sendMessage(const uint8_t msg[], const uint8_t size) const noexcept override;
        void sendSysEx(const audio::midi::MIDIEvent& e) const noexcept override;
        void pause() const noexcept override;
        void resume() const noexcept override;

        inline std::shared_ptr<hardware::opl::OPL> getOpl() const noexcept { return _oplDriver->getOpl(); };
        //void loadBankOP2();
    protected:
        // TODO review the constructors and use a load bank instead..
        /** @deprecated */
        [[deprecated("use the other constructor that creates the OPL chip internally (still used on device::scummvm)")]]
        explicit MidiOpl(
            const std::shared_ptr<hardware::opl::OPL>& opl,
            const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank,
            const audio::mixer::eChannelGroup group,
            const uint8_t volume,
            const uint8_t pan);

        explicit MidiOpl(
            const hardware::opl::OplType type,
            const hardware::opl::OplEmulator emuType,
            const std::shared_ptr<audio::IMixer>& mixer,
            const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank,
            const audio::mixer::eChannelGroup group,
            const uint8_t volume,
            const uint8_t pan);
        ~MidiOpl() override = default;

    private:
        // TODO: i think this can be unique
        // TODO instead of OplDriver use IMidiDriver here
        // TODO: merge with MidiScummVM
        std::shared_ptr<drivers::midi::opl::OplDriver> _oplDriver;
    };
}
