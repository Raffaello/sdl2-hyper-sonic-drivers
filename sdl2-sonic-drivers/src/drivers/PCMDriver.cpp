#include <drivers/PCMDriver.hpp>
#include <audio/scummvm/MixerImpl.hpp>

namespace drivers
{
    using audio::scummvm::Mixer;

    PCMDriver::PCMDriver(std::shared_ptr<audio::scummvm::Mixer> mixer, const int max_channels) :
        _mixer(mixer), _cur_handles(0)
    {
        _max_handles = std::min(mixer->MAX_CHANNELS, max_channels);
        _handles.resize(_max_handles);
    }
   
    bool PCMDriver::isPlaying(const std::shared_ptr<audio::Sound> sound) const noexcept
    {
        for (int i = 0; i < _max_handles; i++) {
            if (_handles[i].s == sound) {
                return _mixer->isSoundHandleActive(_handles[i].h);
            }
        }

        return false;
    }

    void PCMDriver::play(const std::shared_ptr<audio::Sound> sound, const uint8_t volume)
    {
        if (_cur_handles == _max_handles) {
            // clean finished sounds
            for (int i = _max_handles-1; i >=0 ; --i) {
                if (!_mixer->isSoundHandleActive(_handles[i].h)) {
                    _handles[i].s = nullptr;
                    _handles[i].buf_pos = 0;
                    _cur_handles = i;
                }
            }
        }


        //if (_handles[_cur_handles].s != nullptr) {
        //    // here should never go...
        //    for (int i = _cur_handles + 1; i < _max_handles; i++) {
        //        if (_handles[i].s == nullptr) {
        //            _cur_handles = i;
        //            break;
        //        }
        //    }
        //}

        // TODO review it,
        // BODY those constants and could be done in Sound class instead?
        // BODY this driver at the moment became just an helper class. not really usefull..

        if (_cur_handles == _max_handles)
            return;

        channel_t ch = _handles[_cur_handles++];
        ch.s = sound;
        ch.buf_pos = 0;
        _mixer->playStream(
            sound->getSoundType(),
            &ch.h,
            sound.get(),
            -1,
            volume,
            0,
            false
        );
       
        
    }
}
