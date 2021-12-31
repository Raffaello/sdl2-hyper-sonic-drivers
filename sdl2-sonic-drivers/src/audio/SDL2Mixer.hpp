#pragma once

#include <audio/scummvm/Mixer.hpp>
#include <cstdint>
#include <audio/scummvm/SoundHandle.hpp>
#include <audio/scummvm/Timestamp.hpp>
#include <audio/scummvm/AudioStream.hpp>

namespace audio
{
    // TODO rename MixMixerManager (using SDL_Mixer)
    // This Class is not included in the library
    class SDL2Mixer : public scummvm::Mixer
    {
    private:
        int _rate;
        int8_t _bitsDepth;
    
    public:
        SDL2Mixer();
        virtual bool isReady() override;
        virtual void playStream(
            SoundType type,
            scummvm::SoundHandle* handle,
            scummvm::AudioStream* stream,
            int id = -1,
            uint8_t volume = MaxVolume::CHANNEL,
            int8_t balance = 0,
            bool autofreeStream = true,
            bool permanent = false,
            bool reverseStereo = false) override;
        virtual void stopAll() override;
        virtual void stopID(int id) override;
        virtual void stopHandle(scummvm::SoundHandle handle) override;
        virtual void pauseAll(bool paused) override;
        virtual void pauseID(int id, bool paused) override;
        virtual void pauseHandle(scummvm::SoundHandle handle, bool paused) override;
        virtual bool isSoundIDActive(int id) override;
        virtual int getSoundID(scummvm::SoundHandle handle) override;
        virtual bool isSoundHandleActive(scummvm::SoundHandle handle) override;
        virtual void muteSoundType(SoundType type, bool mute) override;
        virtual bool isSoundTypeMuted(SoundType type) const override;
        virtual void setChannelVolume(scummvm::SoundHandle handle, uint8_t volume) override;
        virtual uint8_t getChannelVolume(scummvm::SoundHandle handle) override;
        virtual void setChannelBalance(scummvm::SoundHandle handle, int8_t balance) override;
        virtual int8_t getChannelBalance(scummvm::SoundHandle handle) override;
        virtual uint32_t getSoundElapsedTime(scummvm::SoundHandle handle) override;
        virtual scummvm::Timestamp getElapsedTime(scummvm::SoundHandle handle) override;
        virtual bool hasActiveChannelOfType(SoundType type) override;
        virtual void setVolumeForSoundType(SoundType type, int volume) override;
        virtual int getVolumeForSoundType(SoundType type) const override;
        virtual unsigned int getOutputRate() const override;
        virtual uint8_t getBitsDepth() const override;
    };
}
