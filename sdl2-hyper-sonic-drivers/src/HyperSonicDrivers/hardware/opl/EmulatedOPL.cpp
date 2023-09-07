#include <HyperSonicDrivers/hardware/opl/EmulatedOPL.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>
#include <cassert>

namespace HyperSonicDrivers::hardware::opl
{
    constexpr int FIXP_SHIFT = 16;

    size_t EmulatedOPL::Stream::readBuffer(int16_t* buffer, const size_t numSamples)
    {
        const int stereoFactor = stereo ? 2 : 1;
        size_t len = numSamples / stereoFactor;

        do {
            size_t step = len;
            if (step > (m_nextTick >> FIXP_SHIFT))
            {
                step = (m_nextTick >> FIXP_SHIFT);
            }

            m_opl->generateSamples(buffer, step * stereoFactor);

            m_nextTick -= step << FIXP_SHIFT;
            if (!(m_nextTick >> FIXP_SHIFT))
            {
                if (m_opl->_callback.get() != nullptr)
                    (*m_opl->_callback)();

                m_nextTick += m_samplesPerTick;
            }

            buffer += step * stereoFactor;
            len -= step;
        } while (len);

        return numSamples;
    }

    EmulatedOPL::EmulatedOPL(const OplType type, const std::shared_ptr<audio::IMixer>& mixer) : OPL(type),
        m_mixer(mixer)
    {
    }

    EmulatedOPL::~EmulatedOPL()
    {
        // Stop callbacks, just in case. If it's still playing at this
        // point, there's probably a bigger issue, though. The subclass
        // needs to call stop() or the pointer can still in use by
        // the mixer thread at the same time.
        stop();
    }

    uint32_t EmulatedOPL::setCallbackFrequency(int timerFrequency)
    {
        const uint32_t baseFreq = timerFrequency;
        assert(baseFreq != 0);

        int d = m_mixer->getOutputRate() / baseFreq;
        int r = m_mixer->getOutputRate() % baseFreq;

        // This is equivalent to (getRate() << FIXP_SHIFT) / BASE_FREQ
        // but less prone to arithmetic overflow.
        return (d << FIXP_SHIFT) + (r << FIXP_SHIFT) / baseFreq;
    }

    std::shared_ptr<audio::IMixer> EmulatedOPL::getMixer() const noexcept
    {
        return m_mixer;
    }

    void EmulatedOPL::startCallbacks(int timerFrequency)
    {
        m_stream = std::make_shared<Stream>(
            this,
            isStereo(),
            m_mixer->getOutputRate(),
            setCallbackFrequency(timerFrequency)
        );

        m_channel_id = m_mixer->play(
            audio::mixer::eChannelGroup::Plain,
            m_stream,
            audio::mixer::Channel_max_volume,
            0,
            // TODO: reverseStereo flag instead of false
            false
        );

        if (!m_channel_id.has_value())
            utils::logC("can't start opl playback");
    }

    void EmulatedOPL::stopCallbacks()
    {
        if (m_channel_id.has_value())
        {
            m_mixer->reset(m_channel_id.value());
            m_channel_id = std::nullopt;
        }
    }
}
