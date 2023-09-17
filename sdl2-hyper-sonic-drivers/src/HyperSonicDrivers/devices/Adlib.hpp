#pragma once

#include <HyperSonicDrivers/devices/Device.hpp>
#include <HyperSonicDrivers/drivers/westwood/ADLDriver.hpp>
#include <HyperSonicDrivers/drivers/MIDDriver.hpp>
#include <HyperSonicDrivers/hardware/opl/OPL.hpp>
#include <HyperSonicDrivers/audio/IMixer.hpp>
#include <HyperSonicDrivers/audio/mixer/ChannelGroup.hpp>
#include <HyperSonicDrivers/hardware/opl/OplEmulator.hpp>
#include <HyperSonicDrivers/drivers/midi/devices/Adlib.hpp> // TODO: try to merge...
#include <cstdint>
#include <memory>

namespace HyperSonicDrivers::devices
{
    // TODO: the Adlib sound card for SFX shoudl use PCSpeaker.
    //       as it was not able to play digi-sound (sfx, speech)
    //       but..... anyway.
    //       just create a Sound Blaster that was adlib (OPL2) with digi sound

    class Adlib : public Device
    {
    public:
        Adlib(Adlib&) = delete;
        Adlib(Adlib&&) = delete;
        Adlib& operator=(Adlib&) = delete;

        Adlib(
            const std::shared_ptr<audio::IMixer>& mixer,
            const audio::mixer::eChannelGroup group,
            const hardware::opl::OplEmulator emulator,
            const uint8_t volume,
            const uint8_t pan
        );

        ~Adlib() override = default;

        //void play()

    protected:
        // the "to the parent" means move to Deivce, also not really used after constructor...
        std::shared_ptr<audio::IMixer> m_mixer; // to the parent
        const audio::mixer::eChannelGroup m_group; // to the parent
        const uint8_t m_volume; // to the parent
        const uint8_t m_pan; // to the parent
    private:
        const hardware::opl::OplEmulator m_opl_emulator;
        std::shared_ptr<hardware::opl::OPL> m_opl;
        std::unique_ptr<drivers::westwood::ADLDriver> m_adl_drv;
        // TODO: MIDDrv is using midi::devices, and those must be integrated here... or  not?
        //       for now just finishing this interface/adlib "general device" then let see.
        std::unique_ptr<drivers::MIDDriver> m_mid_drv;
        std::unique_ptr<drivers::midi::devices::Adlib> m_midi_adlib;

    };
}
