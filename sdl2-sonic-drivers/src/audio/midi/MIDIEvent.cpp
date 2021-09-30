#include <audio/midi/MIDIEvent.hpp>

namespace audio
{
    namespace midi
    {
        MIDIEvent::MIDIEvent() : delta_time(0)
        {
            type.val = 0;
        }
    }
}
