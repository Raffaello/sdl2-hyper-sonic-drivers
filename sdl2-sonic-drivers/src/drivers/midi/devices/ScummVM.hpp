#pragma once

#include <drivers/midi/Device.hpp>
#include <drivers/midi/scummvm/MidiDriver_ADLIB.hpp>
#include <memory>
#include <cstdint>
#include <hardware/opl/OPL.hpp>


namespace drivers
{
    namespace midi
    {
        namespace devices
        {
            /**
             * @brief Wrapper around ScummVM MidiDriver (MidiDriver_ADLIB)
             * At the moment support only OPL
             * Better rename to OPL?
            */
            class ScummVM : public Device
            {
            public:
                explicit ScummVM(const std::shared_ptr<hardware::opl::OPL>& opl, const bool opl3mode);
                ~ScummVM();

                void sendEvent(const audio::midi::MIDIEvent& e) const noexcept override;
                void sendMessage(const uint8_t msg[], const uint8_t size) const noexcept override;
                void sendSysEx(const audio::midi::MIDIEvent& e) const noexcept override;
                void pause() const noexcept override;
                void resume() const noexcept override;

            private:
                std::shared_ptr<drivers::midi::scummvm::MidiDriver_ADLIB> _adlib;
            };
        }
    }
}
