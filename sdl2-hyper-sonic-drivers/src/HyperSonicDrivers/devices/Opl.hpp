#pragma once

#include <cstdint>
#include <memory>
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
            //const audio::mixer::eChannelGroup group,
            const hardware::opl::OplEmulator emulator,
            const hardware::opl::OplType type,
            const uint8_t volume,
            const uint8_t pan
        );

    public:
        //void playAdl();
        //void playMidi();

        inline std::shared_ptr<hardware::opl::OPL> getOpl() const noexcept { return m_opl; };

    protected:
        //const hardware::opl::OplEmulator m_opl_emulator;
        std::shared_ptr<hardware::opl::OPL> m_opl;
    };
}
