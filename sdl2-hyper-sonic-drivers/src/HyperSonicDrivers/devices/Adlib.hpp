#pragma once

#include <HyperSonicDrivers/devices/Opl.hpp>
#include <HyperSonicDrivers/drivers/westwood/ADLDriver.hpp>
#include <HyperSonicDrivers/drivers/MIDDriver.hpp>
#include <HyperSonicDrivers/hardware/opl/OPL.hpp>
#include <HyperSonicDrivers/audio/IMixer.hpp>
#include <HyperSonicDrivers/audio/mixer/ChannelGroup.hpp>
#include <HyperSonicDrivers/hardware/opl/OplEmulator.hpp>
//#include <HyperSonicDrivers/drivers/midi/devices/Adlib.hpp> // TODO: try to merge...
#include <cstdint>
#include <memory>

namespace HyperSonicDrivers::devices
{
    // TODO: the Adlib sound card for SFX shoudl use PCSpeaker.
    //       as it was not able to play digi-sound (sfx, speech)
    //       but..... anyway.
    //       just create a Sound Blaster that was adlib (OPL2) with digi sound

    class Adlib : public Opl
    {
    public:
        Adlib(Adlib&) = delete;
        Adlib(Adlib&&) = delete;
        Adlib& operator=(Adlib&) = delete;

        explicit Adlib(
            const std::shared_ptr<audio::IMixer>& mixer,
            //const audio::mixer::eChannelGroup group,
            const hardware::opl::OplEmulator emulator,
            const uint8_t volume = 255,
            const uint8_t pan = 0
        );

        ~Adlib() override = default;

        //void play()

    private:
    };
}
