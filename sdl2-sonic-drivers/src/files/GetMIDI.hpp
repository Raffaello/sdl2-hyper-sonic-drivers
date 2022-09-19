#pragma once
#include <memory>
#include <audio/MIDI.hpp>

namespace files
{
    class GetMIDI
    {
    public:
        /**
        * @brief if the file is not a single track, it will return a
        *        converted equivalent with single track.
        */
        virtual std::shared_ptr<audio::MIDI> getMIDI() const = 0;
    };
}
