#pragma once

#include <cstdint>
#include <mutex>
#include <array>
#include <HyperSonicDrivers/audio/scummvm/Mixer.hpp>
#include <HyperSonicDrivers/audio/scummvm/Channel.hpp>

namespace HyperSonicDrivers::audio::scummvm
{
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

        std::array<SoundTypeSettings, 4> _soundTypeSettings;
        Channel* _channels[NUM_CHANNELS];
    public:
        MixerImpl(unsigned int sampleRate, const uint8_t bitsDepth);
        ~MixerImpl();

        bool isReady() override;

        //std::mutex& mutex() override;

        void playStream(
            SoundType type,
            SoundHandle* handle,
            AudioStream* input,
            int id, uint8_t volume, int8_t balance,
            bool autofreeStream,
            bool permanent,
            bool reverseStereo) override;

        void stopAll() override;
        void stopID(int id) override;
        void stopHandle(SoundHandle handle) override;

        void pauseAll(bool paused) override;
        void pauseID(int id, bool paused) override;
        void pauseHandle(SoundHandle handle, bool paused) override;

        bool isSoundIDActive(int id) override;
        int getSoundID(SoundHandle handle) override;

        bool isSoundHandleActive(SoundHandle handle) override;

        void muteSoundType(SoundType type, bool mute) override;
        bool isSoundTypeMuted(SoundType type) const override;

        void setChannelVolume(SoundHandle handle, uint8_t volume) override;
        uint8_t getChannelVolume(SoundHandle handle) override;
        void setChannelBalance(SoundHandle handle, int8_t balance) override;
        int8_t getChannelBalance(SoundHandle handle) override;

        uint32_t getSoundElapsedTime(SoundHandle handle) override;
        Timestamp getElapsedTime(SoundHandle handle) override;

        bool hasActiveChannelOfType(SoundType type) override;

        void setVolumeForSoundType(SoundType type, int volume) override;
        int getVolumeForSoundType(SoundType type) const override;

        unsigned int getOutputRate() const noexcept override;

        uint8_t getBitsDepth() const override;

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
