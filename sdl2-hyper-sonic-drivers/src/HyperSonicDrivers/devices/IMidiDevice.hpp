#pragma once

#include <cstdint>
#include <atomic>
#include <memory>
#include <functional>
#include <HyperSonicDrivers/audio/midi/MIDIEvent.hpp>


namespace HyperSonicDrivers
{
    namespace drivers
    {
        class MIDDriver;
    }

    namespace devices
    {
        // TODO this can be merged into Device
        //      or added as aggregate.. better merging it i think..
        class IMidiDevice
        {
        public:
            IMidiDevice() = default;
            virtual ~IMidiDevice() = default;

            // TODO: integrate MT-32 first as a device,
            //       init for what?
            //virtual bool init() noexcept = 0;

            virtual void sendEvent(const audio::midi::MIDIEvent& e) const noexcept = 0;
            virtual void sendMessage(const uint8_t msg[], const uint8_t size) const noexcept = 0;
            virtual void sendSysEx(const audio::midi::MIDIEvent& e) const noexcept = 0;
            virtual void pause() const noexcept = 0;
            virtual void resume() const noexcept = 0;

            inline bool isAcquired() const noexcept { return _acquired; }
            inline bool isOwned(const /*void**/ drivers::MIDDriver* owner) const noexcept { return _owner == owner; }

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

                // maybe here should always return false... it can be true just because is not acquired i guess...
                return !isAcquired();
            }
        private:
            std::atomic<bool> _acquired = false;
            std::atomic<drivers::MIDDriver*> _owner = nullptr;
        };
    }
}
