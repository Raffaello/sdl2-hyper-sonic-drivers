#pragma once

#include <audio/scummvm/Mixer.hpp>
#include <audio/scummvm/Channel.hpp>
#include <mutex>
#include <cstdint>

namespace HyperSonicDrivers::audio::scummvm
{
    // TODO: This is temprary mixer implementation
    //       to be integrated in SDL2Mixer class hen working.

    /**
    * @defgroup audio_mixer_intern Mixer implementation
    * @ingroup audio
    *
    * @brief The (default) implementation of the ScummVM audio mixing subsystem.
    * @{
    */

    /**
    * The (default) implementation of the ScummVM audio mixing subsystem.
    *
    * Backends are responsible for allocating (and later releasing) an instance
    * of this class, which engines can access via OSystem::getMixer().
    *
    * Initialisation of instances of this class usually happens as follows:
    * 1) Creat a new Audio::MixerImpl instance.
    * 2) Set the hardware output sample rate via the setSampleRate() method.
    * 3) Hook up the mixCallback() in a suitable audio processing thread/callback.
    * 4) Change the mixer into ready mode via setReady(true).
    * 5) Start audio processing (e.g. by resuming the audio thread, if applicable).
    *
    * In the future, we might make it possible for backends to provide
    * (partial) alternative implementations of the mixer, e.g. to make
    * better use of native sound mixing support on low-end devices.
    *
    * @see OSystem::getMixer()
    */
    class MixerImpl : public Mixer
    {
    private:
        enum {
            NUM_CHANNELS = MIXER_MAX_CHANNELS
        };

        //Common::Mutex _mutex;
        std::mutex _mutex;

        const unsigned int _sampleRate;
        uint8_t _bitsDepth;
        bool _mixerReady;
        uint32_t _handleSeed;

        struct SoundTypeSettings
        {
            SoundTypeSettings() : mute(false), volume(MaxVolume::MIXER) {}

            bool mute;
            int volume;
        };

        SoundTypeSettings _soundTypeSettings[4];
        Channel* _channels[NUM_CHANNELS];
    public:

        MixerImpl(unsigned int sampleRate, const uint8_t bitsDepth);
        ~MixerImpl();

        virtual bool isReady() override;

        virtual std::mutex& mutex();

        virtual void playStream(
            SoundType type,
            SoundHandle* handle,
            AudioStream* input,
            int id, uint8_t volume, int8_t balance,
            bool autofreeStream,
            bool permanent,
            bool reverseStereo) override;

        virtual void stopAll() override;
        virtual void stopID(int id) override;
        virtual void stopHandle(SoundHandle handle) override;

        virtual void pauseAll(bool paused) override;
        virtual void pauseID(int id, bool paused) override;
        virtual void pauseHandle(SoundHandle handle, bool paused) override;

        virtual bool isSoundIDActive(int id) override;
        virtual int getSoundID(SoundHandle handle) override;

        virtual bool isSoundHandleActive(SoundHandle handle) override;

        virtual void muteSoundType(SoundType type, bool mute) override;
        virtual bool isSoundTypeMuted(SoundType type) const override;

        virtual void setChannelVolume(SoundHandle handle, uint8_t volume) override;
        virtual uint8_t getChannelVolume(SoundHandle handle) override;
        virtual void setChannelBalance(SoundHandle handle, int8_t balance) override;
        virtual int8_t getChannelBalance(SoundHandle handle) override;

        virtual uint32_t getSoundElapsedTime(SoundHandle handle) override;
        virtual Timestamp getElapsedTime(SoundHandle handle) override;

        virtual bool hasActiveChannelOfType(SoundType type) override;

        virtual void setVolumeForSoundType(SoundType type, int volume) override;
        virtual int getVolumeForSoundType(SoundType type) const override;

        virtual unsigned int getOutputRate() const noexcept override;

        virtual uint8_t getBitsDepth() const override;

    protected:
        void insertChannel(SoundHandle* handle, Channel* chan);

    public:
        /**
         * The mixer callback function, to be called at regular intervals by
         * the backend (e.g. from an audio mixing thread). All the actual mixing
         * work is done from here.
         *
         * @param samples Sample buffer, in which stereo 16-bit samples will be stored.
         * @param len Length of the provided buffer to fill (in bytes, should be divisible by 4).
         * @return number of sample pairs processed (which can still be silence!)
         */
        int mixCallback(uint8_t* samples, unsigned int len);

        /**
         * Set the internal 'is ready' flag of the mixer.
         * Backends should invoke Mixer::setReady(true) once initialisation of
         * their audio system has been completed.
         */
        void setReady(bool ready);
    };
}
