#pragma once
#include <audio/scummvm/Mixer.hpp>
#include <audio/scummvm/MixerImpl.hpp>

namespace audio
{
    namespace scummvm
    {

        /**
         * Abstract class for mixer manager. Subclasses
         * implement the real functionality.
         */
        class MixerManager
        {
        public:
            MixerManager() : _mixer(0), _audioSuspended(false) {}
            virtual ~MixerManager() { delete _mixer; }

            /**
             * Initialize and setups the mixer
             */
            virtual void init() = 0;

            /**
             * Get the audio mixer implementation
             */
            Mixer* getMixer() { return (Mixer*)_mixer; }

            // Used by LinuxMoto Port

            /**
             * Pauses the audio system
             */
            virtual void suspendAudio() = 0;

            /**
             * Resumes the audio system
             */
            virtual int resumeAudio() = 0;

        protected:
            /** The mixer implementation */
            MixerImpl* _mixer;

            /** State of the audio system */
            bool _audioSuspended;
        };
    }
}
