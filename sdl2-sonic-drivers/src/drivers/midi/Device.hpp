#pragma once

#include <audio/midi/MIDIEvent.hpp>
#include <cstdint>
#include <atomic>


// TODO: namespace drivers::midi::devices could be considered
//       to be replaced instead as devices:: and devices::midi ?
namespace HyperSonicDrivers::drivers
{
    class MIDDriver;

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

            inline bool isAcquired() const noexcept { return _acquired; }
            inline bool isOwned(const /*void**/ drivers::MIDDriver* owner) const noexcept { return _owner == owner; }

            // TODO: a binary semaphore could be also used i suppose...
            inline bool acquire(/*void**/ drivers::MIDDriver* owner)
            {
                if (!_acquired) {
                    _acquired = true;
                    _owner = owner;
                    return true;
                }
                else return false;
            }

            bool release(const /*void**/ drivers::MIDDriver* owner)
            {
                if (isOwned(owner)) {
                    _acquired = false;
                    return true;
                }

                // maybe here should always return false... it can be true just because is not aquired i guess...
                return !isAcquired();
            }
        private:
            // TODO: this could be replaced with a unique_lock mutex instead?
            std::atomic<bool> _acquired = false;
            std::atomic<drivers::MIDDriver*> _owner = nullptr;
        };
    }
}
