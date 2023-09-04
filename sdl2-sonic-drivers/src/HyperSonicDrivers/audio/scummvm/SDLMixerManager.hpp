#pragma once

#include <cstdint>
#include <HyperSonicDrivers/audio/scummvm/MixerManager.hpp>
#include <SDL2/SDL_audio.h>

namespace HyperSonicDrivers::audio::scummvm
{
    /**
     * SDL mixer manager. It wraps the actual implementation
     * of the Audio:Mixer used by the engine, and setups
     * the SDL audio subsystem and the callback for the
     * audio mixer implementation.
     */
    class SdlMixerManager : public MixerManager
    {
    public:
        virtual ~SdlMixerManager();

        /**
         * Initialize and setups the mixer
         */
        virtual void init();

        // Used by Event recorder

        /**
         * Pauses the audio system
         */
        virtual void suspendAudio();

        /**
         * Resumes the audio system
         */
        virtual int resumeAudio();

    protected:
        /**
         * The obtained audio specification after opening the
         * audio system.
         */
        SDL_AudioSpec _obtained = {};

        /**
         * Returns the desired audio specification
         */
        virtual SDL_AudioSpec getAudioSpec(uint32_t rate);

        /**
         * Starts SDL audio
         */
        virtual void startAudio();

        /**
         * Handles the audio callback
         */
        virtual void callbackHandler(uint8_t* samples, int len);

        /**
         * The mixer callback entry point. Static functions can't be overrided
         * by subclasses, so it invokes the non-static function callbackHandler()
         */
        static void sdlCallback(void* this_, uint8_t* samples, int len);
    };
}
