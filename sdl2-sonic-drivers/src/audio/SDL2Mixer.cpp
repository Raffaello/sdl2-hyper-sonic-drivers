#include <audio/SDL2Mixer.hpp>

namespace audio
{
    bool SDL2Mixer::isReady() const
    {
        return false;
    }
    void SDL2Mixer::playStream(SoundType type, scummvm::SoundHandle* handle, scummvm::AudioStream* stream, int id, uint8_t volume, int8_t balance, bool autofreeStream, bool permanent, bool reverseStereo)
    {
    }
    void SDL2Mixer::stopAll()
    {
    }
    void SDL2Mixer::stopID(int id)
    {
    }
    void SDL2Mixer::stopHandle(scummvm::SoundHandle handle)
    {
    }
    void SDL2Mixer::pauseAll(bool paused)
    {
    }
    void SDL2Mixer::pauseID(int id, bool paused)
    {
    }
    void SDL2Mixer::pauseHandle(scummvm::SoundHandle handle, bool paused)
    {
    }
    bool SDL2Mixer::isSoundIDActive(int id)
    {
        return false;
    }
    int SDL2Mixer::getSoundID(scummvm::SoundHandle handle)
    {
        return 0;
    }
    bool SDL2Mixer::isSoundHandleActive(scummvm::SoundHandle handle)
    {
        return false;
    }
    void SDL2Mixer::muteSoundType(SoundType type, bool mute)
    {
    }
    bool SDL2Mixer::isSoundTypeMuted(SoundType type) const
    {
        return false;
    }
    void SDL2Mixer::setChannelVolume(scummvm::SoundHandle handle, uint8_t volume)
    {
    }
    uint8_t SDL2Mixer::getChannelVolume(scummvm::SoundHandle handle)
    {
        return uint8_t();
    }
    void SDL2Mixer::setChannelBalance(scummvm::SoundHandle handle, int8_t balance)
    {
    }
    int8_t SDL2Mixer::getChannelBalance(scummvm::SoundHandle handle)
    {
        return int8_t();
    }
    uint32_t SDL2Mixer::getSoundElapsedTime(scummvm::SoundHandle handle)
    {
        return uint32_t();
    }
    scummvm::Timestamp SDL2Mixer::getElapsedTime(scummvm::SoundHandle handle)
    {
        return scummvm::Timestamp();
    }
    bool SDL2Mixer::hasActiveChannelOfType(SoundType type)
    {
        return false;
    }
    void SDL2Mixer::setVolumeForSoundType(SoundType type, int volume)
    {
    }
    int SDL2Mixer::getVolumeForSoundType(SoundType type) const
    {
        return 0;
    }
    unsigned int SDL2Mixer::getOutputRate() const
    {
        return 0;
    }
}
