#pragma once
#include <audio/scummvm/Mixer.hpp>
#include <audio/scummvm/MixerImpl.hpp>
#include <memory>

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
            MixerManager() : _mixer(nullptr), _audioSuspended(false) {}
            virtual ~MixerManager() {};

            /**
             * Initialize and setups the mixer
             */
            virtual void init() = 0;

            /**
             * Get the audio mixer implementation
             */
            //Mixer* getMixer() { return (Mixer*)_mixer.get(); }
            std::shared_ptr<Mixer> getMixer() { return _mixer; }

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
            std::shared_ptr<MixerImpl> _mixer;

            /** State of the audio system */
            bool _audioSuspended;
        };
    }
}
