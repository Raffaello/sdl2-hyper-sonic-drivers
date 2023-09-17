#pragma once

#include <cstdint>
#include <memory>
#include <HyperSonicDrivers/audio/IMixer.hpp>
#include <HyperSonicDrivers/audio/mixer/ChannelGroup.hpp>
#include <HyperSonicDrivers/devices/IDevice.hpp>
#include <HyperSonicDrivers/drivers/westwood/ADLDriver.hpp>
#include <HyperSonicDrivers/hardware/opl/OPL.hpp>
#include <HyperSonicDrivers/hardware/opl/OplEmulator.hpp>
#include <HyperSonicDrivers/hardware/opl/OplType.hpp>
//#include <HyperSonicDrivers/drivers/MIDDriver.hpp>
//#include <HyperSonicDrivers/drivers/midi/devices/Adlib.hpp> // TODO: try to merge...


namespace HyperSonicDrivers::devices
{
    class Opl : public IDevice
    {
    protected:
        explicit Opl(
            const std::shared_ptr<audio::IMixer>& mixer,
            const audio::mixer::eChannelGroup group,
            const hardware::opl::OplEmulator emulator,
            const hardware::opl::OplType type,
            const uint8_t volume,
            const uint8_t pan
        );


    protected:
        const hardware::opl::OplEmulator m_opl_emulator;
        std::shared_ptr<hardware::opl::OPL> m_opl;
        std::unique_ptr<drivers::westwood::ADLDriver> m_adl_drv;

        // TODO: MIDDrv is using midi::devices, and those must be integrated here... or  not?
        //       for now just finishing this interface/adlib "general device" then let see.
        //std::unique_ptr<drivers::MIDDriver> m_mid_drv;
        //std::unique_ptr<drivers::midi::devices::Adlib> m_midi_adlib;
    };
}
