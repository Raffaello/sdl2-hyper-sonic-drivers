#include <audio/scummvm/Channel.hpp>

namespace audio
{
    namespace scummvm
    {
        Channel::Channel(Mixer* mixer, Mixer::SoundType type, AudioStream* stream,
            DisposeAfterUse::Flag autofreeStream, bool reverseStereo, int id, bool permanent)
            : _type(type), _mixer(mixer), _id(id), _permanent(permanent), _volume(Mixer::kMaxChannelVolume),
            _balance(0), _pauseLevel(0), _samplesConsumed(0), _samplesDecoded(0), _mixerTimeStamp(0),
            _pauseStartTime(0), _pauseTime(0), _converter(0), _volL(0), _volR(0),
            _stream(stream, autofreeStream) {
            assert(mixer);
            assert(stream);

            // Get a rate converter instance
            _converter = makeRateConverter(_stream->getRate(), mixer->getOutputRate(), _stream->isStereo(), reverseStereo);
        }

        Channel::~Channel() {
            delete _converter;
        }

        void Channel::setVolume(const byte volume) {
            _volume = volume;
            updateChannelVolumes();
        }

        byte Channel::getVolume() {
            return _volume;
        }

        void Channel::setBalance(const int8 balance) {
            _balance = balance;
            updateChannelVolumes();
        }

        int8 Channel::getBalance() {
            return _balance;
        }

        void Channel::updateChannelVolumes() {
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
                    _volL = vol / Mixer::kMaxChannelVolume;
                    _volR = vol / Mixer::kMaxChannelVolume;
                }
                else if (_balance < 0) {
                    _volL = vol / Mixer::kMaxChannelVolume;
                    _volR = ((127 + _balance) * vol) / (Mixer::kMaxChannelVolume * 127);
                }
                else {
                    _volL = ((127 - _balance) * vol) / (Mixer::kMaxChannelVolume * 127);
                    _volR = vol / Mixer::kMaxChannelVolume;
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

                if (_pauseLevel == 1)
                    _pauseStartTime = g_system->getMillis(true);
            }
            else if (_pauseLevel > 0) {
                _pauseLevel--;

                if (!_pauseLevel) {
                    _pauseTime = (g_system->getMillis(true) - _pauseStartTime);
                    _pauseStartTime = 0;
                }
            }
        }

        Timestamp Channel::getElapsedTime() {
            const uint32 rate = _mixer->getOutputRate();
            uint32 delta = 0;

            Audio::Timestamp ts(0, rate);

            if (_mixerTimeStamp == 0)
                return ts;

            if (isPaused())
                delta = _pauseStartTime - _mixerTimeStamp;
            else
                delta = g_system->getMillis(true) - _mixerTimeStamp - _pauseTime;

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

        int Channel::mix(int16* data, uint len) {
            assert(_stream);

            int res = 0;
            if (_stream->endOfData()) {
                // TODO: call drain method
            }
            else {
                assert(_converter);
                _samplesConsumed = _samplesDecoded;
                _mixerTimeStamp = g_system->getMillis(true);
                _pauseTime = 0;
                res = _converter->flow(*_stream, data, len, _volL, _volR);
                _samplesDecoded += res;
            }

            return res;
        }

    }
}
