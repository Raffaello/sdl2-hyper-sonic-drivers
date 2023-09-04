#pragma once

#include <memory>
#include <HyperSonicDrivers/audio/scummvm/Mixer.hpp>
#include <HyperSonicDrivers/audio/scummvm/MixerImpl.hpp>

namespace HyperSonicDrivers::audio::scummvm
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
        std::shared_ptr<Mixer> getMixer() { return _mixer; }

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
