#include <HyperSonicDrivers/audio/streams/EmulatedStream.hpp>

namespace HyperSonicDrivers::audio::streams
{
    using hardware::FIXP_SHIFT;

    EmulatedStream::EmulatedStream(
        hardware::IHardware* hw,
        const bool stereo, const uint32_t rate, const uint32_t samplesPerTick) :
        m_hw(hw), stereo(stereo), rate(rate), samplesPerTick(samplesPerTick)
    {
    }

    size_t EmulatedStream::readBuffer(int16_t* buffer, const size_t numSamples)
    {
        const int stereoFactor = stereo ? 2 : 1;
        size_t len = numSamples / stereoFactor;

        do {
            size_t step = len;
            if (step > (m_nextTick >> FIXP_SHIFT))
            {
                step = (m_nextTick >> FIXP_SHIFT);
            }

            m_hw->generateSamples(buffer, step);

            m_nextTick -= step << FIXP_SHIFT;
            if (!(m_nextTick >> FIXP_SHIFT))
            {
                m_hw->callCallback();
                m_nextTick += samplesPerTick;
            }

            buffer += step * stereoFactor;
            len -= step;
        } while (len);

        return numSamples;
    }
}
