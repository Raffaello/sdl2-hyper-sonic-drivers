#pragma once

#include <audio/midi/MIDIEvent.hpp>
#include <cstdint>


// TODO: namespace drivers::midi::devices could be considered
//       to be replaced instead as devices:: and devices::midi ?
namespace drivers
{
    namespace midi
    {
        class Device
        {
        public:
            Device() = default;
            ~Device() = default;

            virtual void sendEvent(const audio::midi::MIDIEvent& e) const noexcept = 0;
            virtual void sendMessage(const uint8_t msg[], const uint8_t size) const noexcept = 0;
            virtual void sendSysEx(const audio::midi::MIDIEvent& e) const noexcept = 0;
            virtual void pause() const noexcept = 0;
            virtual void resume() const noexcept = 0;

            // DRAFT:
            // TODO use an acquire/release mechanism to be used from the driver
            // to allow to be used simultaneously only 1 at time from the drivers
            inline bool acquire() { 
                // todo who acquired?
                // it might be enough, but can be hacked
                if (!_acquired) {
                    _acquired = true;
                    return true;
                }
                else return false;
            }

            // TODO
            // it is enough to call release() to acquire from another device.
            // this mechanism doesn't work
            bool release() { _acquired = false; return true; }
        private:
            bool _acquired = false;
        };
    }
}
