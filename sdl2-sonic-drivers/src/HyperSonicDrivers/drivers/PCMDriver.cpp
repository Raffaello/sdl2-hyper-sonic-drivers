#include <HyperSonicDrivers/drivers/PCMDriver.hpp>
#include <HyperSonicDrivers/audio/scummvm/MixerImpl.hpp>
#include <HyperSonicDrivers/audio/scummvm/Mixer.hpp>

namespace HyperSonicDrivers::drivers
{
    using audio::scummvm::Mixer;
    using audio::streams::SoundStream;

    PCMDriver::PCMDriver(const std::shared_ptr<audio::scummvm::Mixer>& mixer, const int max_channels) :
        _mixer(mixer)
    {
        _max_streams = std::min(audio::scummvm::MIXER_MAX_CHANNELS, max_channels);
        _soundStreams.resize(_max_streams);
    }

    PCMDriver::~PCMDriver()
    {
    }
   
    bool PCMDriver::isPlaying() const noexcept
    {
        for (int i = 0; i < _max_streams; ++i) {
            if (isSoundHandleActive(i))
                return true;
        }

        return false;
    }

    bool PCMDriver::isPlaying(const std::shared_ptr<audio::Sound>& sound) const noexcept
    {
        // TODO: should be returned the soundHandle or soundID in play method to be used later on?
        // BODY: so here it can be addressed in constant time instead of searching for sound in the slots?
        for (int i = 0; i < _max_streams; i++) {
            if (nullptr != _soundStreams[i] && _soundStreams[i]->getSound().lock() == sound) {
                return isSoundHandleActive(i);
            }
        }

        return false;
    }

    void PCMDriver::play(const std::shared_ptr<audio::Sound>& sound, const uint8_t volume, const int8_t balance)
    {
        // TODO: this method is not thread-safe at the moment.
        int cur_stream;
        
        // find first free slot
        for (cur_stream = 0; cur_stream < _max_streams ; ++cur_stream) {
            if(!isSoundHandleActive(cur_stream))
                break;
        }

        if (cur_stream == _max_streams)
            return;
        
        _soundStreams[cur_stream] = std::make_shared<SoundStream>(SoundStream(sound));
        
        // TODO: could be autofree stream and create directly on the playStream method simplified all?
        // BODY: Yes, but loosing the handle for checking if is it playing.
        // BODY: alternatively could be stored the ID?
        _mixer->playStream(
            sound->soundType,
            _soundStreams[cur_stream]->getSoundHandlePtr(),
            _soundStreams[cur_stream].get(),
            -1,
            volume,
            balance,
            false
        );
    }

    inline bool PCMDriver::isSoundHandleActive(const int index) const noexcept
    {
        return nullptr != _soundStreams[index] &&
            _mixer->isSoundHandleActive(*_soundStreams[index]->getSoundHandlePtr());
    }
}
