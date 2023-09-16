#pragma once

#include <cstdint>
#include <memory>
#include <HyperSonicDrivers/drivers/midi/Device.hpp>
#include <HyperSonicDrivers/drivers/midi/scummvm/MidiDriver_ADLIB.hpp>
#include <HyperSonicDrivers/hardware/opl/OPL.hpp>


namespace HyperSonicDrivers::drivers::midi::devices
{
    /**
     * @brief Wrapper around ScummVM MidiDriver (MidiDriver_ADLIB)
     * At the moment support only OPL
     * TODO: this is more a driver than a device...
     * TODO: MI.MID percussions sounds not right
    */
    class [[deprecated]] ScummVM : public Device
    {
    public:
        [[deprecated]]
        explicit ScummVM(const std::shared_ptr<hardware::opl::OPL>& opl, const bool opl3mode,
            const audio::mixer::eChannelGroup group,
            const uint8_t volume = 255,
            const uint8_t pan = 0);

        ~ScummVM() override;

        void sendEvent(const audio::midi::MIDIEvent& e) const noexcept override;
        void sendMessage(const uint8_t msg[], const uint8_t size) const noexcept override;
        void sendSysEx(const audio::midi::MIDIEvent& e) const noexcept override;
        void pause() const noexcept override;
        void resume() const noexcept override;

    private:
        std::shared_ptr<drivers::midi::scummvm::MidiDriver_ADLIB> _adlib;
    };
}
