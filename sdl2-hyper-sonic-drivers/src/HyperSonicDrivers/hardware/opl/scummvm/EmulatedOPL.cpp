#include <HyperSonicDrivers/hardware/opl/scummvm/EmulatedOPL.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>
#include <cassert>

namespace HyperSonicDrivers::hardware::opl::scummvm
{
    constexpr int FIXP_SHIFT = 16;

    EmulatedOPL::EmulatedOPL(const OplType type, const std::shared_ptr<audio::IMixer>& mixer) : OPL(type),
        m_mixer(mixer)
    {
    }

    EmulatedOPL::~EmulatedOPL()
    {
        // Stop callbacks, just in case. If it's still playing at this
        // point, there's probably a bigger issue, though. The subclass
        // needs to call stop() or the pointer can still use be used in
        // the mixer thread at the same time.
        stop();
        //if (m_channel_id.has_value())
        //{
            //m_mixer->unlockChannelStream(m_channel_id.value());
        //}
    }

    void EmulatedOPL::setCallbackFrequency(int timerFrequency)
    {
        _baseFreq = timerFrequency;
        assert(_baseFreq != 0);

        int d = m_self->getRate() / _baseFreq;
        int r = m_self->getRate() % _baseFreq;

        // This is equivalent to (getRate() << FIXP_SHIFT) / BASE_FREQ
        // but less prone to arithmetic overflow.
        _samplesPerTick = (d << FIXP_SHIFT) + (r << FIXP_SHIFT) / _baseFreq;
    }

    size_t EmulatedOPL::Stream::readBuffer(int16_t* buffer, const size_t numSamples)
    {
        const int stereoFactor = /*isStereo()*/ stereo ? 2 : 1;
        size_t len = numSamples / stereoFactor;

        do {
            size_t step = len;
            if (step > (m_opl->_nextTick >> FIXP_SHIFT))
            {
                step = (m_opl->_nextTick >> FIXP_SHIFT);
            }

            m_opl->generateSamples(buffer, step * stereoFactor);

            m_opl->_nextTick -= step << FIXP_SHIFT;
            if (!(m_opl->_nextTick >> FIXP_SHIFT))
            {
                if (m_opl->_callback.get() != nullptr)
                    (*m_opl->_callback)();

                m_opl->_nextTick += m_opl->_samplesPerTick;
            }

            buffer += step * stereoFactor;
            len -= step;
        } while (len);

        return numSamples;
    }

    /*uint32_t EmulatedOPL::Stream::getRate() const noexcept
    {
        return m_opl->m_mixer->getOutputRate();
    }*/

    /*bool EmulatedOPL::Stream::endOfData() const noexcept
    {
        return false;
    }*/

    std::shared_ptr<audio::IMixer> EmulatedOPL::getMixer() const noexcept
    {
        return m_mixer;
    }

    void EmulatedOPL::startCallbacks(int timerFrequency)
    {
        m_self = std::make_shared<Stream>(this, isStereo(), m_mixer->getOutputRate());
        setCallbackFrequency(timerFrequency);

        m_channel_id = m_mixer->play(
            audio::mixer::eChannelGroup::Plain,
            m_self,
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
        if (m_channel_id.has_value()) {
            m_mixer->reset(m_channel_id.value());
            m_channel_id = std::nullopt;
        }
    }
}
