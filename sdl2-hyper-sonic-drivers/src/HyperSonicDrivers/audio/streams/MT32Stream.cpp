#include <HyperSonicDrivers/audio/streams/MT32Stream.hpp>

namespace HyperSonicDrivers::audio::streams
{
    MT32Stream::MT32Stream(
        hardware::mt32::MT32* mt32,
        const bool stereo, const uint32_t rate, cosnt uint32_t samplePerTick
    ) : m_mt32(mt32), stereo(stereo), rate(rate), samplePerTick(samplePerTick)
    {
    }

    size_t MT32Stream::readBuffer(int16_t* buffer, const size_t numSamples)
    {
        const int stereoFactor = isStereo() ? 2 : 1;
        size_t len = numSamples / stereoFactor;

        do {
            size_t step = len;
            if (step > (_nextTick >> FIXP_SHIFT))
            {
                step = (_nextTick >> FIXP_SHIFT);
            }

            generateSamples(data, step);

            _nextTick -= step << FIXP_SHIFT;
            if (!(_nextTick >> FIXP_SHIFT))
            {

                if (_timerProc)
                    (*_timerProc)(_timerParam);

                //onTimer();

                _nextTick += _samplesPerTick;
            }

            data += step * stereoFactor;
            len -= step;
        } while (len);

        return numSamples;
    }

    bool MT32Stream::isStereo() const
    {
        return false;
    }
    uint32_t MT32Stream::getRate() const
    {
        return 0;
    }
    bool MT32Stream::endOfData() const
    {
        return false;
    }

    
}
