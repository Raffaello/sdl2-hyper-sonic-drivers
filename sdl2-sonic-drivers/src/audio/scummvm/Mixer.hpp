#pragma once

#include <mutex>
#include <audio/scummvm/Timestamp.hpp>
#include <audio/scummvm/SoundHandle.hpp>
#include <audio/scummvm/AudioStream.hpp>

namespace HyperSonicDrivers::audio::scummvm
{
    constexpr int MIXER_MAX_CHANNELS = 32;

    class Mixer
    {
    public:
        Mixer(const Mixer&) = delete;
        Mixer& operator=(const Mixer&) = delete;

        /** Sound types. */
        enum class SoundType
        {
            PLAIN = 0, /*!< Plain sound. */
            MUSIC = 1, /*!< Music. */
            SFX = 2,   /*!< Sound effects. */
            SPEECH = 3 /*!< Speech. */
        };


        /** Max volumes. */
        enum MaxVolume
        {
            CHANNEL = 255, /*!< Max channel volume. */
            MIXER = 256    /*!< Max global volume. */
        };

        Mixer() {}
        virtual ~Mixer() {}

        /**
         * Check whether the mixer is ready and set up.
         *
         * The mixer might not be set up on systems that do not support
         * digital sound output. In such case, the mixer processing might
         * never be called. That, in turn, can cause breakage in games that try to
         * sync with an audio stream. In particular, the AdLib MIDI emulation.
         *
         * @return Whether the mixer is ready and set up.
         *
         * @todo get rid of this?
         */
        virtual bool isReady() = 0;

        /**
         * Return the mixer's internal mutex so that audio players can use it.
         */
         //virtual Common::Mutex& mutex() = 0;

         /**
          * Start playing the given audio stream.
          *
          * Note that the sound ID assigned here is unique. At most, one stream
          * with the given ID can play at any given time. Trying to play a sound
          * with an ID that is already in use causes the new sound to not be played.
          *
          * @param type      Type of the stream - voice/SFX/music.
          * @param handle    A SoundHandle instance that can be used to reference and control
          *                  the stream using suitable mixer methods.
          * @param stream    The actual AudioStream to be played.
          * @param id        Unique ID assigned to this stream.
          * @param volume    Volume with which to play the sound, ranging from 0 to 255.
          * @param balance	Balance with which to play the sound, ranging from -127 to 127 (full left to full right).
          *                  0 is balanced, -128 is invalid.
          * @param autofreeStream  If set, the stream will be freed after the playback is finished.
          * @param permanent       If set, a plain stopAll call will not stop this particular stream.
          * @param reverseStereo   If set, left and right channels will be swapped.
          *
          * TODO: remove the Pointers
          */
        virtual void playStream(
            SoundType type,
            SoundHandle* handle,
            AudioStream* stream,
            int id = -1,
            uint8_t volume = MaxVolume::CHANNEL,
            int8_t balance = 0,
            bool autofreeStream = true,
            bool permanent = false,
            bool reverseStereo = false) = 0;

        /**
         * Stop all currently playing sounds.
         */
        virtual void stopAll() = 0;

        /**
         * Stop playing the sound with the given ID.
         *
         * @param id  ID of the sound.
         */
        virtual void stopID(int id) = 0;

        /**
         * Stop playing the sound corresponding to the given handle.
         *
         * @param handle  The sound to stop playing.
         */
        virtual void stopHandle(SoundHandle handle) = 0;

        /**
         * Pause or unpause all sounds, including all regular and permanent
         * channels.
         *
         * @param paused  True to pause everything, false to unpause.
         */
        virtual void pauseAll(bool paused) = 0;

        /**
         * Pause or unpause the sound with the given ID.
         *
         * @param id      ID of the sound.
         * @param paused  True to pause the sound, false to unpause it.
         */
        virtual void pauseID(int id, bool paused) = 0;

        /**
         * Pause or unpause the sound corresponding to the given handle.
         *
         * @param handle  The sound to pause or unpause.
         * @param paused  True to pause the sound, false to unpause it.
         */
        virtual void pauseHandle(SoundHandle handle, bool paused) = 0;

        /**
         * Check whether a sound with the given ID is active.
         *
         * @param id  ID of the sound to query.
         *
         * @return True if the sound is active.
         */
        virtual bool isSoundIDActive(int id) = 0;

        /**
         * Get the sound ID for the given handle.
         *
         * @param handle The sound to query.
         *
         * @return Sound ID if the sound is active.
         */
        virtual int getSoundID(SoundHandle handle) = 0;

        /**
         * Check whether a sound with the given handle is active.
         *
         * @param handle The sound to query.
         *
         * @return True if the sound is active.
         */
        virtual bool isSoundHandleActive(SoundHandle handle) = 0;

        /**
         * Set the mute state for a given sound type.
         *
         * @param type Sound type. See @ref SoundType.
         * @param mute Whether to mute (= true) or not (= false).
         */
        virtual void muteSoundType(SoundType type, bool mute) = 0;

        /**
         * Query the mute state for a given sound type.
         *
         * @param type Sound type. See @ref SoundType.
         */
        virtual bool isSoundTypeMuted(SoundType type) const = 0;

        /**
         * Set the channel volume for the given handle.
         *
         * @param handle  The sound to affect.
         * @param volume  The new channel volume, in the range 0 - kMaxChannelVolume.
         */
        virtual void setChannelVolume(SoundHandle handle, uint8_t volume) = 0;

        /**
         * Get the channel volume for the given handle.
         *
         * @param handle  The sound to affect.
         *
         * @return The channel volume.
         */
        virtual uint8_t getChannelVolume(SoundHandle handle) = 0;

        /**
         * Set the channel balance for the given handle.
         *
         * @param handle   The sound to affect.
         * @param balance  The new channel balance:
         *                 (-127 ... 0 ... 127) corresponds to (left ... center ... right)
         */
        virtual void setChannelBalance(SoundHandle handle, int8_t balance) = 0;

        /**
         * Get the channel balance for the given handle.
         *
         * @param handle  The sound to affect.
         *
         * @return The channel balance.
         */
        virtual int8_t getChannelBalance(SoundHandle handle) = 0;

        /**
         * Get an approximation of for how long the channel has been playing.
         */
        virtual uint32_t getSoundElapsedTime(SoundHandle handle) = 0;

        /**
         * Get an approximation of for how long the channel has been playing.
         */
        virtual Timestamp getElapsedTime(SoundHandle handle) = 0;

        /**
         * Check whether any channel of the given sound type is active.
         *
         * For example, this can be used to check whether any SFX sound
         * is currently playing by checking for type kSFXSoundType.
         *
         * @param  type  The sound type to query.
         *
         * @return True if any channels of the specified type are active.
         */
        virtual bool hasActiveChannelOfType(SoundType type) = 0;

        /**
         * Set the volume for the given sound type.
         *
         * @param type    Sound type.
         * @param volume  The new global volume, in the range 0 - kMaxMixerVolume.
         */
        virtual void setVolumeForSoundType(SoundType type, int volume) = 0;

        /**
         * Check what the global volume is for a sound type.
         *
         * @param type  Sound type.
         *
         * @return The global volume, in the range 0 - kMaxMixerVolume.
         */
        virtual int getVolumeForSoundType(SoundType type) const = 0;

        /**
         * Return the output sample rate of the system.
         *
         * @return The output sample rate in Hz.
         */
        virtual unsigned int getOutputRate() const = 0;

        /**
         * Return how many bits are used for one sample.
         *
         * @return one sample size in bits.
         */
        virtual uint8_t getBitsDepth() const = 0;
    };
}
