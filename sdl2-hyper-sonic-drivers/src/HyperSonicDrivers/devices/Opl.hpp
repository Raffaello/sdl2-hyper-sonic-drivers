#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <HyperSonicDrivers/audio/IMixer.hpp>
#include <HyperSonicDrivers/audio/mixer/ChannelGroup.hpp>
#include <HyperSonicDrivers/devices/IDevice.hpp>
#include <HyperSonicDrivers/hardware/opl/OPL.hpp>
#include <HyperSonicDrivers/hardware/opl/OplEmulator.hpp>
#include <HyperSonicDrivers/hardware/opl/OplType.hpp>
#include <HyperSonicDrivers/audio/opl/banks/OP2Bank.hpp>


namespace HyperSonicDrivers::devices
{
    class Opl : public IDevice
    {
    protected:
        explicit Opl(
            const std::shared_ptr<audio::IMixer>& mixer,
            const hardware::opl::OplEmulator emulator,
            const hardware::opl::OplType type,
            const uint8_t volume,
            const uint8_t pan
        );

    public:
        /*bool loadBankOP2(
            const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank,
            const audio::mixer::eChannelGroup group,
            const uint8_t volume,
            const uint8_t pan) noexcept;*/

        bool init() noexcept override;
        bool shutdown() noexcept override;

        //void sendEvent(const audio::midi::MIDIEvent& e) const noexcept override;
        //void sendMessage(const uint8_t msg[], const uint8_t size) const noexcept override;
        //void sendSysEx(const audio::midi::MIDIEvent& e) const noexcept override;
        //void pause() const noexcept override;
        //void resume() const noexcept override;

        inline std::optional<uint8_t> getChannelId() const noexcept override { return m_opl->getChannelId(); };

        //void playAdl();
        //void playMidi();

        inline std::shared_ptr<hardware::opl::OPL> getOpl() const noexcept { return m_opl; };
        hardware::opl::OPL* getHardware() const noexcept override { return dynamic_cast<hardware::opl::OPL*>(IDevice::getHardware()); };
    private:
        std::shared_ptr<hardware::opl::OPL> m_opl;
    };
}
