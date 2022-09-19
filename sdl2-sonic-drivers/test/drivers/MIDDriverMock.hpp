#pragma once

#include <audio/midi/MIDITrack.hpp>
#include <audio/scummvm/Mixer.hpp>
#include <drivers/midi/Device.hpp>
#include <drivers/MIDDriver.hpp>
#include <memory>

namespace drivers
{
    class MIDDriverMock : public MIDDriver
    {
    public:
        explicit MIDDriverMock(const std::shared_ptr<audio::scummvm::Mixer>& mixer, const std::shared_ptr<midi::Device>& device) :
            MIDDriver(mixer, device)
        {

        }

        void protected_processTrack(const audio::midi::MIDITrack& track, const uint16_t division)
        {
            processTrack(track, division);
        }
    };
}
