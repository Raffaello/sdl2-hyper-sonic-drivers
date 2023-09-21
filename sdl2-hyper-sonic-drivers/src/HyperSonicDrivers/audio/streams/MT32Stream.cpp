#include <HyperSonicDrivers/audio/streams/MT32Stream.hpp>

namespace HyperSonicDrivers::audio::streams
{
    using hardware::FIXP_SHIFT;

    MT32Stream::MT32Stream(
        hardware::mt32::MT32* mt32,
        const bool stereo, const uint32_t rate, const uint32_t samplesPerTick
    ) : m_mt32(mt32), stereo(stereo), rate(rate), samplesPerTick(samplesPerTick)
    {
    }

    size_t MT32Stream::readBuffer(int16_t* buffer, const size_t numSamples)
    {
        const int stereoFactor = isStereo() ? 2 : 1;
        size_t len = numSamples / stereoFactor;

        do {
            size_t step = len;
            if (step > (m_nextTick >> FIXP_SHIFT))
            {
                step = (m_nextTick >> FIXP_SHIFT);
            }

            m_mt32->generateSamples(buffer, step);

            m_nextTick -= step << FIXP_SHIFT;
            if (!(m_nextTick >> FIXP_SHIFT))
            {
                m_mt32->callCallback();

                //onTimer();

                m_nextTick += samplesPerTick;
            }

            buffer += step * stereoFactor;
            len -= step;
        } while (len);

        return numSamples;
    }
}
