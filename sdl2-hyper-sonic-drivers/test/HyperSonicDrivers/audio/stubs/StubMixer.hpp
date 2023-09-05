#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <HyperSonicDrivers/audio/IAudioStream.hpp>
#include <HyperSonicDrivers/audio/scummvm/Mixer.hpp>
#include <HyperSonicDrivers/audio/scummvm/Timestamp.hpp>

namespace HyperSonicDrivers::audio::stubs
{
    class StubMixer : public scummvm::Mixer
    {
    public:
        int rate = 44100;

        virtual bool isReady() override
        {
            return true;
        };
        virtual void playStream(
            SoundType type,
            scummvm::SoundHandle* handle,
            IAudioStream* stream,
            int id = -1,
            uint8_t volume = MaxVolume::CHANNEL,
            int8_t balance = 0,
            bool autofreeStream = true,
            bool permanent = false,
            bool reverseStereo = false) override
        {};
        virtual void stopAll() override
        {};
        virtual void stopID(int id) override
        {};
        virtual void stopHandle(scummvm::SoundHandle handle) override
        {};
        virtual void pauseAll(bool paused) override
        {}
        virtual void pauseID(int id, bool paused) override
        {}
        virtual void pauseHandle(scummvm::SoundHandle handle, bool paused) override
        {}
        virtual bool isSoundIDActive(int id) override
        {
            return true;
        }
        virtual int getSoundID(scummvm::SoundHandle handle) override
        {
            return 0;
        }
        virtual bool isSoundHandleActive(scummvm::SoundHandle handle) override
        {
            return true;
        }
        virtual void muteSoundType(SoundType type, bool mute) override
        {}
        virtual bool isSoundTypeMuted(SoundType type) const override
        {
            return true;
        }
        virtual void setChannelVolume(scummvm::SoundHandle handle, uint8_t volume) override
        {}
        virtual uint8_t getChannelVolume(scummvm::SoundHandle handle) override
        {
            return 0;
        }
        virtual void setChannelBalance(scummvm::SoundHandle handle, int8_t balance) override
        {}
        virtual int8_t getChannelBalance(scummvm::SoundHandle handle) override
        {
            return 0;
        }
        virtual uint32_t getSoundElapsedTime(scummvm::SoundHandle handle) override
        {
            return 0;
        }
        virtual scummvm::Timestamp getElapsedTime(scummvm::SoundHandle handle) override
        {
            return scummvm::Timestamp();
        }
        virtual bool hasActiveChannelOfType(SoundType type) override
        {
            return true;
        };
        virtual void setVolumeForSoundType(SoundType type, int volume) override
        {};
        virtual int getVolumeForSoundType(SoundType type) const override
        {
            return 0;
        };
        virtual unsigned int getOutputRate() const override
        {
            return rate;
        }
        virtual uint8_t getBitsDepth() const override
        {
            return 16;
        }
    };
}
