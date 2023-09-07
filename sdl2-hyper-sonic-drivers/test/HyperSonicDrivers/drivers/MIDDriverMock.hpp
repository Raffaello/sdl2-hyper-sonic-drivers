#pragma once

#include <HyperSonicDrivers/audio/midi/MIDITrack.hpp>
#include <HyperSonicDrivers/drivers/midi/Device.hpp>
#include <HyperSonicDrivers/drivers/MIDDriver.hpp>
#include <memory>

namespace HyperSonicDrivers::drivers
{
    class MIDDriverMock : public MIDDriver
    {
    public:
        explicit MIDDriverMock(const std::shared_ptr<midi::Device>& device) :
            MIDDriver(device)
        {

        }

        void protected_processTrack(const audio::midi::MIDITrack& track, const uint16_t division)
        {
            processTrack(track, division);
        }
    };
}
