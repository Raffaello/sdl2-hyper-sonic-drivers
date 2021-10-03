#include <drivers/PCMDriver.hpp>
#include <audio/scummvm/MixerImpl.hpp>

namespace drivers
{
    using audio::scummvm::Mixer;
    using audio::streams::SoundStream;

    PCMDriver::PCMDriver(std::shared_ptr<audio::scummvm::Mixer> mixer, const int max_channels) :
        _mixer(mixer)
    {
        _max_streams = std::min(mixer->MAX_CHANNELS, max_channels);
        _soundStreams.resize(_max_streams);
    }

    PCMDriver::~PCMDriver()
    {
    }
   
    bool PCMDriver::isPlaying(const std::shared_ptr<audio::Sound> sound) const noexcept
    {
        for (int i = 0; i < _max_streams; i++) {
            if (nullptr != _soundStreams[i] && _soundStreams[i]->getSound().lock() == sound) {
                return _mixer->isSoundHandleActive(*_soundStreams[i]->getSoundHandlePtr());
            }
        }

        return false;
    }

    void PCMDriver::play(const std::shared_ptr<audio::Sound> sound, const uint8_t volume, const int8_t balance)
    {
        // TODO: is not thread-safe now
        int cur_stream ;
        // find first free slot
        for (cur_stream = 0; cur_stream < _max_streams ; ++cur_stream) {
            if (nullptr == _soundStreams[cur_stream])
                break;

            if (!_mixer->isSoundHandleActive(*_soundStreams[cur_stream]->getSoundHandlePtr())) {
                break;
            }
        }

        // TODO review it,
        // BODY those constants and could be done in Sound class instead?
        // BODY this driver at the moment became just an helper class. not really usefull..
        if (cur_stream == _max_streams)
            return;

        // TODO use SoundStream, create a Soundstream from the sound arg
        _soundStreams[cur_stream] = std::make_shared<SoundStream>(SoundStream(sound));
        // TODO: could be autofree stream and create directly on the playStream method simplified all?
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
}
