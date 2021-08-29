#include <hardware/opl/scummvm/EmulatedOPL.hpp>
#include <cassert>

namespace hardware
{
    namespace opl
    {
        namespace scummvm
        {
            constexpr int FIXP_SHIFT = 16;


            EmulatedOPL::EmulatedOPL(const std::shared_ptr<audio::scummvm::Mixer> mixer) : OPL(),
                _mixer(mixer)
            {
                _handle = new audio::scummvm::SoundHandle();
            }

            EmulatedOPL::~EmulatedOPL()
            {
                // Stop callbacks, just in case. If it's still playing at this
                // point, there's probably a bigger issue, though. The subclass
                // needs to call stop() or the pointer can still use be used in
                // the mixer thread at the same time.
                stop();

                delete _handle;
            }

            void EmulatedOPL::setCallbackFrequency(int timerFrequency)
            {
                _baseFreq = timerFrequency;
                assert(_baseFreq != 0);

                int d = getRate() / _baseFreq;
                int r = getRate() % _baseFreq;

                // This is equivalent to (getRate() << FIXP_SHIFT) / BASE_FREQ
                // but less prone to arithmetic overflow.
                _samplesPerTick = (d << FIXP_SHIFT) + (r << FIXP_SHIFT) / _baseFreq;
            }

            int EmulatedOPL::readBuffer(int16_t* buffer, const int numSamples)
            {
                const int stereoFactor = isStereo() ? 2 : 1;
                int len = numSamples / stereoFactor;
                int step;

                do {
                    step = len;
                    if (step > (_nextTick >> FIXP_SHIFT)) {
                        step = (_nextTick >> FIXP_SHIFT);
                    }

                    generateSamples(buffer, step * stereoFactor);

                    _nextTick -= step << FIXP_SHIFT;
                    if (!(_nextTick >> FIXP_SHIFT)) {
                        if (_callback.get() != nullptr)
                            (*_callback)();

                        _nextTick += _samplesPerTick;
                    }

                    buffer += step * stereoFactor;
                    len -= step;
                } while (len);

                return numSamples;
            }

            int EmulatedOPL::getRate() const
            {
                return _mixer->getOutputRate();
            }

            bool EmulatedOPL::endOfData() const noexcept
            {
                return false;
            }

            const std::shared_ptr<audio::scummvm::Mixer> EmulatedOPL::getMixer()
            {
                return _mixer;
            }

            void EmulatedOPL::startCallbacks(int timerFrequency)
            {
                setCallbackFrequency(timerFrequency);
                _mixer->playStream(
                    audio::scummvm::Mixer::SoundType::PLAIN,
                    _handle,
                    this,
                    -1,
                    audio::scummvm::Mixer::MaxVolume::CHANNEL,
                    0,
                    false,
                    true
                );
            }

            void EmulatedOPL::stopCallbacks()
            {
                _mixer->stopHandle(*_handle);
            }
        }
    }
}

