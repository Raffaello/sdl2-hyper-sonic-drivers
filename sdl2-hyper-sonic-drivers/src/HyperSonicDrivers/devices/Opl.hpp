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
        inline std::optional<uint8_t> getChannelId() const noexcept override { return m_opl->getChannelId(); };

        //void playAdl();
        //void playMidi();

        inline std::shared_ptr<hardware::opl::OPL> getOpl() const noexcept { return m_opl; };

    private:
        std::shared_ptr<hardware::opl::OPL> m_opl;
    };
}
