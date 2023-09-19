#pragma once

#include <HyperSonicDrivers/audio/midi/MIDIEvent.hpp>
#include <HyperSonicDrivers/devices/IMidiDevice.hpp>

namespace HyperSonicDrivers::devices::midi
{
    class SpyMidiDevice : public IMidiDevice
    {
        virtual void sendEvent(const audio::midi::MIDIEvent& e) const noexcept override
        {

        };
        virtual void sendMessage(const uint8_t msg[], const uint8_t size) const noexcept override
        {

        }
        virtual void sendSysEx(const audio::midi::MIDIEvent& e) const noexcept override
        {

        }
        virtual void pause() const noexcept override
        {

        }
        virtual void resume() const noexcept override
        {

        }
    };
}

