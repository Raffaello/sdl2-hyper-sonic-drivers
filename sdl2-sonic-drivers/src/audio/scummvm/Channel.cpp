#include <audio/scummvm/Channel.hpp>
#include <audio/scummvm/Timestamp.hpp>
#include <cassert>
#include <chrono>
#include <utils/algorithms.hpp>


namespace audio
{
    namespace scummvm
    {
        Channel::Channel(Mixer* mixer, Mixer::SoundType type, AudioStream* stream, bool autofreeStream, bool reverseStereo, int id, bool permanent)
            : _type(type), _mixer(mixer), _id(id), _permanent(permanent), _volume(Mixer::MaxVolume::CHANNEL),
            _balance(0), _pauseLevel(0), _samplesConsumed(0), _samplesDecoded(0), _mixerTimeStamp(0),
            _pauseStartTime(0), _pauseTime(0), _converter(nullptr), _volL(0), _volR(0),
            _stream(nullptr)
        {
            assert(mixer);
            assert(stream);

            _stream = stream;
            _dispose_stream = autofreeStream;
            // Get a rate converter instance
            _converter = makeRateConverter(_stream->getRate(), mixer->getOutputRate(), _stream->isStereo(), reverseStereo);
        }

        Channel::~Channel() {
            if (_converter != nullptr) {
                delete _converter;
                _converter = nullptr;
            }

            if (_dispose_stream && _stream != nullptr) {
                delete _stream;
                _stream = nullptr;
            }
        }

        void Channel::setVolume(const uint8_t volume) {
            _volume = volume;
            updateChannelVolumes();
        }

        uint8_t Channel::getVolume() {
            return _volume;
        }

        void Channel::setBalance(const int8_t balance) {
            _balance = balance;
            updateChannelVolumes();
        }

        int8_t Channel::getBalance() {
            return _balance;
        }

        void Channel::updateChannelVolumes()
        {
            // From the channel balance/volume and the global volume, we compute
            // the effective volume for the left and right channel. Note the
            // slightly odd divisor: the 255 reflects the fact that the maximal
            // value for _volume is 255, while the 127 is there because the
            // balance value ranges from -127 to 127.  The mixer (music/sound)
            // volume is in the range 0 - kMaxMixerVolume.
            // Hence, the vol_l/vol_r values will be in that range, too

            if (!_mixer->isSoundTypeMuted(_type)) {
                int vol = _mixer->getVolumeForSoundType(_type) * _volume;

                if (_balance == 0) {
                    _volL = vol / Mixer::MaxVolume::CHANNEL;
                    _volR = vol / Mixer::MaxVolume::MIXER;
                }
                else if (_balance < 0) {
                    _volL = vol / Mixer::MaxVolume::CHANNEL;
                    _volR = ((127 + _balance) * vol) / (Mixer::MaxVolume::CHANNEL * 127);
                }
                else {
                    _volL = ((127 - _balance) * vol) / (Mixer::MaxVolume::CHANNEL * 127);
                    _volR = vol / Mixer::MaxVolume::CHANNEL;
                }
            }
            else {
                _volL = _volR = 0;
            }
        }

        void Channel::pause(bool paused) {
            //assert((paused && _pauseLevel >= 0) || (!paused && _pauseLevel));

            if (paused) {
                _pauseLevel++;

                if (_pauseLevel == 1) {
                    //_pauseStartTime = g_system->getMillis(true);
                    _pauseStartTime = utils::getMillis<int32_t>();
                }
            }
            else if (_pauseLevel > 0) {
                _pauseLevel--;

                if (!_pauseLevel) {
                    _pauseTime = (utils::getMillis<int32_t>() - _pauseStartTime);
                    _pauseStartTime = 0;
                }
            }
        }

        Timestamp Channel::getElapsedTime()
        {
            const uint32_t rate = _mixer->getOutputRate();
            uint32_t delta = 0;

            Timestamp ts(0, rate);

            if (_mixerTimeStamp == 0)
                return ts;

            if (isPaused())
                delta = _pauseStartTime - _mixerTimeStamp;
            else
                delta = utils::getMillis<int32_t>() - _mixerTimeStamp - _pauseTime;

            // Convert the number of samples into a time duration.

            ts = ts.addFrames(_samplesConsumed);
            ts = ts.addMsecs(delta);

            // In theory it would seem like a good idea to limit the approximation
            // so that it never exceeds the theoretical upper bound set by
            // _samplesDecoded. Meanwhile, back in the real world, doing so makes
            // the Broken Sword cutscenes noticeably jerkier. I guess the mixer
            // isn't invoked at the regular intervals that I first imagined.

            return ts;
        }

        int Channel::mix(int16_t* data, unsigned int len)
        {
            assert(_stream);

            int res = 0;
            if (_stream->endOfData()) {
                // TODO: call drain method
            }
            else {
                assert(_converter);
                _samplesConsumed = _samplesDecoded;
                _mixerTimeStamp = utils::getMillis<int32_t>();
                _pauseTime = 0;
                res = _converter->flow(*_stream, data, len, _volL, _volR);
                _samplesDecoded += res;
            }

            return res;
        }
    }
}
