#pragma once

#include <HyperSonicDrivers/audio/midi/MIDITrack.hpp>
#include <HyperSonicDrivers/devices/IDevice.hpp>
#include <HyperSonicDrivers/drivers/MIDDriver.hpp>
#include <memory>

namespace HyperSonicDrivers::drivers
{
    class MIDDriverMock : public MIDDriver
    {
    public:
        explicit MIDDriverMock(const std::shared_ptr<devices::IDevice>& device) :
            MIDDriver(device, audio::mixer::eChannelGroup::Unknown)
        {

        }

        void protected_processTrack(const audio::midi::MIDITrack& track, const uint16_t division)
        {
            processTrack(track, division);
        }
    };
}
