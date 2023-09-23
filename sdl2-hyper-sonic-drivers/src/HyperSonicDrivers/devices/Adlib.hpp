#pragma once

#include <HyperSonicDrivers/devices/Opl.hpp>
#include <HyperSonicDrivers/drivers/westwood/ADLDriver.hpp>
#include <HyperSonicDrivers/drivers/MIDDriver.hpp>
#include <HyperSonicDrivers/hardware/opl/OPL.hpp>
#include <HyperSonicDrivers/audio/IMixer.hpp>
#include <HyperSonicDrivers/audio/mixer/ChannelGroup.hpp>
#include <HyperSonicDrivers/hardware/opl/OplEmulator.hpp>
#include <cstdint>
#include <memory>

namespace HyperSonicDrivers::devices
{
    class Adlib : public Opl
    {
    public:
        //Adlib(Adlib&) = delete;
        //Adlib(Adlib&&) = delete;
        //Adlib& operator=(Adlib&) = delete;

        explicit Adlib(
            const std::shared_ptr<audio::IMixer>& mixer,
            const hardware::opl::OplEmulator emulator = hardware::opl::OplEmulator::AUTO,
            const uint8_t volume = 255,
            const uint8_t pan = 0
        );

        ~Adlib() override = default;
    };
}
