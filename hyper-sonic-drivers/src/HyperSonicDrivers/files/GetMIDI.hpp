#pragma once
#include <memory>
#include <HyperSonicDrivers/audio/MIDI.hpp>

namespace HyperSonicDrivers::files
{
    class GetMIDI
    {
    public:
        virtual ~GetMIDI() = default;
        /**
        * @brief if the file is not a single track, it will return a
        *        converted equivalent with single track.
        */
        virtual std::shared_ptr<audio::MIDI> getMIDI() const = 0;
    };
}
