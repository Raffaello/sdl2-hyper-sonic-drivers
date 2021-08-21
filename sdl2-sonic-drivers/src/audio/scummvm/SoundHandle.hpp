#pragma once

#include <cstdint>

namespace audio
{
    namespace scummvm
    {
        /**
         * A SoundHandle instance corresponds to a specific sound
         * being played using the mixer. It can be used to control that
         * sound (pause it, stop it, etc.).
         * @see Mixer
         */
        class SoundHandle
        {
            friend class Channel;
            //friend class MixerImpl;
            uint32_t _val;
        public:
            inline SoundHandle() : _val(0xFFFFFFFF) {}
        };
    }
}
