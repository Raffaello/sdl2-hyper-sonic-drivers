#include <format>
#include <algorithm>
#include <cassert>
#include <HyperSonicDrivers/audio/scummvm/MixerImpl.hpp>
#include <HyperSonicDrivers/utils/algorithms.hpp>
#include <SDL2/SDL_log.h>


namespace HyperSonicDrivers::audio::scummvm
{
    // TODO: move to utils and as a constexpr
#define ARRAYSIZE(x) ((int)(sizeof(x) / sizeof(x[0])))

    MixerImpl::MixerImpl(unsigned int sampleRate, const uint8_t bitsDepth)
        : _mutex(), _sampleRate(sampleRate), _bitsDepth(bitsDepth),
        _mixerReady(false), _handleSeed(0), _soundTypeSettings()
    {
        assert(sampleRate > 0);

        if (bitsDepth != 16) {
            SDL_LogWarn(SDL_LOG_CATEGORY_AUDIO, std::format("Audio {} bits not supported. Only 16 bits", bitsDepth).c_str());
        }

        for (int i = 0; i != NUM_CHANNELS; i++) {
            _channels[i] = nullptr;
        }
    }

    MixerImpl::~MixerImpl()
    {
        for (int i = 0; i != NUM_CHANNELS; i++) {
            delete _channels[i];
        }
    }

    bool MixerImpl::isReady()
    {
        const std::lock_guard<std::mutex> lock(_mutex);

        return _mixerReady;
    }

    std::mutex& MixerImpl::mutex()
    {
        return _mutex;
    }

    void MixerImpl::playStream(
        SoundType type,
        SoundHandle* handle,
        AudioStream* stream,
        int id, uint8_t volume, int8_t balance,
        bool autofreeStream,
        bool permanent,
        bool reverseStereo)
    {
        const std::lock_guard<std::mutex> lock(_mutex);

        if (stream == 0) {
            SDL_LogWarn(SDL_LOG_CATEGORY_AUDIO, "stream is 0");
            return;
        }

        assert(_mixerReady);

        // Prevent duplicate sounds
        if (id != -1) {
            for (int i = 0; i != NUM_CHANNELS; i++)
                if (_channels[i] != 0 && _channels[i]->getId() == id) {
                    // Delete the stream if were asked to auto-dispose it.
                    // Note: This could cause trouble if the client code does not
                    // yet expect the stream to be gone. The primary example to
                    // keep in mind here is QueuingAudioStream.
                    // Thus, as a quick rule of thumb, you should never, ever,
                    // try to play QueuingAudioStreams with a sound id.
                    if (autofreeStream) {
                        delete stream;
                    }
                    return;
                }
        }

#ifdef AUDIO_REVERSE_STEREO
        reverseStereo = !reverseStereo;
#endif

        // Create the channel
        Channel* chan = new Channel(this, type, stream, autofreeStream, reverseStereo, id, permanent);
        chan->setVolume(volume);
        chan->setBalance(balance);
        insertChannel(handle, chan);
    }

    void MixerImpl::stopAll()
    {
        const std::lock_guard<std::mutex> lock(_mutex);

        for (int i = 0; i != NUM_CHANNELS; i++)
        {
            if (_channels[i] != 0 && !_channels[i]->isPermanent()) {
                delete _channels[i];
                _channels[i] = 0;
            }
        }
    }

    void MixerImpl::stopID(int id)
    {
        const std::lock_guard<std::mutex> lock(_mutex);

        for (int i = 0; i != NUM_CHANNELS; i++)
        {
            if (_channels[i] != 0 && _channels[i]->getId() == id) {
                delete _channels[i];
                _channels[i] = 0;
            }
        }
    }

    void MixerImpl::stopHandle(SoundHandle handle)
    {
        const std::lock_guard<std::mutex> lock(_mutex);

        // Simply ignore stop requests for handles of sounds that already terminated
        const int index = handle._val % NUM_CHANNELS;
        if (!_channels[index] || _channels[index]->getHandle()._val != handle._val) {
            return;
        }

        delete _channels[index];
        _channels[index] = 0;
    }

    void MixerImpl::pauseAll(bool paused)
    {
        const std::lock_guard<std::mutex> lock(_mutex);

        for (int i = 0; i != NUM_CHANNELS; i++)
        {
            if (_channels[i] != 0) {
                _channels[i]->pause(paused);
            }
        }
    }

    void MixerImpl::pauseID(int id, bool paused)
    {
        const std::lock_guard<std::mutex> lock(_mutex);

        for (int i = 0; i != NUM_CHANNELS; i++)
        {
            if (_channels[i] != 0 && _channels[i]->getId() == id) {
                _channels[i]->pause(paused);
                return;
            }
        }
    }

    void MixerImpl::pauseHandle(SoundHandle handle, bool paused)
    {
        const std::lock_guard<std::mutex> lock(_mutex);

        // Simply ignore (un)pause requests for sounds that already terminated
        const int index = handle._val % NUM_CHANNELS;
        if (!_channels[index] || _channels[index]->getHandle()._val != handle._val) {
            return;
        }

        _channels[index]->pause(paused);
    }

    bool MixerImpl::isSoundIDActive(int id)
    {
        const std::lock_guard<std::mutex> lock(_mutex);

#ifdef ENABLE_EVENTRECORDER
        g_eventRec.updateSubsystems();
#endif

        for (int i = 0; i != NUM_CHANNELS; i++)
        {
            if (_channels[i] && _channels[i]->getId() == id) {
                return true;
            }
        }

        return false;
    }

    int MixerImpl::getSoundID(SoundHandle handle)
    {
        const std::lock_guard<std::mutex> lock(_mutex);

        const int index = handle._val % NUM_CHANNELS;
        if (_channels[index] && _channels[index]->getHandle()._val == handle._val) {
            return _channels[index]->getId();
        }

        return 0;
    }

    bool MixerImpl::isSoundHandleActive(SoundHandle handle)
    {
        const std::lock_guard<std::mutex> lock(_mutex);

#ifdef ENABLE_EVENTRECORDER
        g_eventRec.updateSubsystems();
#endif

        const int index = handle._val % NUM_CHANNELS;
        return _channels[index] && _channels[index]->getHandle()._val == handle._val;
    }

    void MixerImpl::muteSoundType(SoundType type, bool mute)
    {
        assert(0 <= (int)type && (int)type < ARRAYSIZE(_soundTypeSettings));
        _soundTypeSettings[static_cast<int>(type)].mute = mute;

        for (int i = 0; i != NUM_CHANNELS; ++i)
        {
            if (_channels[i] && _channels[i]->getType() == type) {
                _channels[i]->notifyGlobalVolChange();
            }
        }
    }

    bool MixerImpl::isSoundTypeMuted(SoundType type) const
    {
        assert(0 <= (int)type && (int)type < ARRAYSIZE(_soundTypeSettings));

        return _soundTypeSettings[static_cast<int>(type)].mute;
    }

    void MixerImpl::setChannelVolume(SoundHandle handle, uint8_t volume)
    {
        const std::lock_guard<std::mutex> lock(_mutex);

        const int index = handle._val % NUM_CHANNELS;
        if (!_channels[index] || _channels[index]->getHandle()._val != handle._val) {
            return;
        }

        _channels[index]->setVolume(volume);
    }

    uint8_t MixerImpl::getChannelVolume(SoundHandle handle)
    {
        const int index = handle._val % NUM_CHANNELS;
        if (!_channels[index] || _channels[index]->getHandle()._val != handle._val)
            return 0;

        return _channels[index]->getVolume();
    }

    void MixerImpl::setChannelBalance(SoundHandle handle, int8_t balance)
    {
        const std::lock_guard<std::mutex> lock(_mutex);

        const int index = handle._val % NUM_CHANNELS;
        if (!_channels[index] || _channels[index]->getHandle()._val != handle._val)
            return;

        _channels[index]->setBalance(balance);
    }

    int8_t MixerImpl::getChannelBalance(SoundHandle handle)
    {
        const int index = handle._val % NUM_CHANNELS;
        if (!_channels[index] || _channels[index]->getHandle()._val != handle._val)
            return 0;

        return _channels[index]->getBalance();
    }

    uint32_t MixerImpl::getSoundElapsedTime(SoundHandle handle) {
        return getElapsedTime(handle).msecs();
    }

    Timestamp MixerImpl::getElapsedTime(SoundHandle handle)
    {
        const std::lock_guard<std::mutex> lock(_mutex);

        const int index = handle._val % NUM_CHANNELS;
        if (!_channels[index] || _channels[index]->getHandle()._val != handle._val)
            return Timestamp(0, _sampleRate);

        return _channels[index]->getElapsedTime();
    }

    bool MixerImpl::hasActiveChannelOfType(SoundType type)
    {
        const std::lock_guard<std::mutex> lock(_mutex);
        for (int i = 0; i != NUM_CHANNELS; i++)
            if (_channels[i] && _channels[i]->getType() == type)
                return true;
        return false;
    }

    void MixerImpl::setVolumeForSoundType(SoundType type, int volume)
    {
        assert(0 <= (int)type && (int)type < ARRAYSIZE(_soundTypeSettings));

        // Check range
        volume = std::clamp<int>(volume, 0, MaxVolume::MIXER);

        // TODO: Maybe we should do logarithmic (not linear) volume
        // scaling? See also Player_V2::setMasterVolume

        const std::lock_guard<std::mutex> lock(_mutex);
        _soundTypeSettings[static_cast<int>(type)].volume = volume;

        for (int i = 0; i != NUM_CHANNELS; ++i)
        {
            if (_channels[i] && _channels[i]->getType() == type)
                _channels[i]->notifyGlobalVolChange();
        }
    }

    int MixerImpl::getVolumeForSoundType(SoundType type) const
    {
        assert(0 <= (int)type && (int)type < ARRAYSIZE(_soundTypeSettings));

        return _soundTypeSettings[static_cast<int>(type)].volume;
    }

    unsigned int MixerImpl::getOutputRate() const noexcept
    {
        return _sampleRate;
    }

    uint8_t MixerImpl::getBitsDepth() const
    {
        return _bitsDepth;
    }

    void MixerImpl::insertChannel(SoundHandle* handle, Channel* chan)
    {
        int index = -1;
        for (int i = 0; i != NUM_CHANNELS; i++)
        {
            if (_channels[i] == 0) {
                index = i;
                break;
            }
        }
        if (index == -1) {
            SDL_LogWarn(SDL_LOG_CATEGORY_AUDIO, "MixerImpl::out of mixer slots");
            delete chan;
            return;
        }

        _channels[index] = chan;

        SoundHandle chanHandle;
        chanHandle._val = index + (_handleSeed * NUM_CHANNELS);

        chan->setHandle(chanHandle);
        _handleSeed++;
        if (handle)
            *handle = chanHandle;
    }

    int MixerImpl::mixCallback(uint8_t* samples, unsigned int len)
    {
        assert(samples);

        const std::lock_guard<std::mutex> lock(_mutex);

        int16_t* buf = reinterpret_cast<int16_t*>(samples);
        // we store stereo, 16-bit samples
        assert(len % 4 == 0);
        len >>= 2;

        // Since the mixer callback has been called, the mixer must be ready...
        _mixerReady = true;

        //  zero the buf
        memset(buf, 0, 2 * len * sizeof(int16_t));

        // mix all channels
        int res = 0, tmp;
        for (int i = 0; i != NUM_CHANNELS; i++)
        {
            if (_channels[i] == nullptr)
                continue;

            if (_channels[i]->isFinished())
            {
                delete _channels[i];
                _channels[i] = 0;
            }
            else if (!_channels[i]->isPaused())
            {
                tmp = _channels[i]->mix(buf, len);

                if (tmp > res)
                    res = tmp;
            }
        }

        return res;
    }

    void MixerImpl::setReady(bool ready)
    {
        const std::lock_guard<std::mutex> lock(_mutex);

        _mixerReady = ready;
    }
}
