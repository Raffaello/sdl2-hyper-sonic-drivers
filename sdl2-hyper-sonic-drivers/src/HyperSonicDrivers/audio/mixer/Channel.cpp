#include <cstdint>
#include <HyperSonicDrivers/audio/mixer/Channel.hpp>
#include <HyperSonicDrivers/audio/mixer/config.hpp>
#include <HyperSonicDrivers/utils/algorithms.hpp>
#include <cassert>

namespace HyperSonicDrivers::audio::mixer
{
    Channel::Channel(IMixer& mixer, const uint8_t id) :
        m_mixer(mixer), m_id(id)
    {
    }

    void Channel::setAudioStream(const mixer::eChannelGroup group, const std::shared_ptr<IAudioStream>& stream, const bool reverseStereo)
    {
        assert(&stream != nullptr && stream.get() != nullptr);
        reset();
        m_group = group;
        m_stream = stream;
        m_converter = converters::makeIRateConverter(m_stream->getRate(), m_mixer.freq, m_stream->isStereo(), reverseStereo);
    }

    void Channel::setAudioStream(const mixer::eChannelGroup group, const std::shared_ptr<IAudioStream>& stream,
        const uint8_t volume,
        const int8_t pan,
        const bool reverseStereo)
    {
        setAudioStream(group, stream, reverseStereo);
        setVolumePan(volume, pan);
    }

    size_t Channel::mix(int16_t* data, size_t len)
    {
        size_t res = 0;

        if (isPaused() || isEnded())
            return res;

        if (m_stream->isEnded())
        {
            // NOTE: drain is doing nothing
            //       not sure what should do.
            //       probably filling in the interpolation missing bytes
            //res = m_converter->drain(nullptr, 0, 0);
            reset();
        }
        else
        {
            //m_samplesConsumed = m_samplesDecoded;
            //m_mixerTimeStamp = utils::getMillis<int32_t>();
            //m_pauseTime = 0;
            res = m_converter->flow(*m_stream, data, len, m_volL, m_volR);
            //m_samplesDecoded += res;
        }

        return res;
    }

    void Channel::pause() noexcept
    {
        if (m_pause)
            return;

        m_pauseTime = 0;
        m_pauseStartTime = utils::getMillis<uint32_t>();
    }

    void Channel::unpause() noexcept
    {
        if (!m_pause)
            return;

        m_pauseTime = utils::getMillis<uint32_t>() - m_pauseStartTime;
        m_pauseStartTime = 0;
    }

    void Channel::reset() noexcept
    {
        m_stream.reset();
        m_converter.reset();
        m_pause = false;
        m_pauseStartTime = m_pauseTime = 0;
        m_volR = m_volL = 0;
        m_volume = 0;
        m_pan = 0;
        m_group = mixer::eChannelGroup::Unknown;
    }

    void Channel::setVolume(const uint8_t volume)
    {
        m_volume = volume;
        updateVolumePan();
    }

    void Channel::setPan(const int8_t pan)
    {
        m_pan = pan;
        updateVolumePan();
    }

    void Channel::setVolumePan(const uint8_t volume, const uint8_t pan)
    {
        m_volume = volume;
        m_pan = pan;
        updateVolumePan();
    }

    void Channel::updateVolumePan()
    {
        if (m_mixer.isChannelGroupMuted(m_group))
        {
            m_volR = m_volL = 0;
            return;
        }

        constexpr float ch_max_vol = static_cast<float>(mixer::Channel_max_volume);
        const uint16_t vol = m_volume * m_mixer.getChannelGroupVolume(m_group);
        const float pan = (127.5f + m_pan) / 255.0f;
        // TODO: create different selectable pan laws
        // -3dB pan law
        m_volL = static_cast<uint16_t>(std::round(sqrt(1 - pan) * vol / ch_max_vol));
        m_volR = static_cast<uint16_t>(std::round(sqrt(pan) * vol / ch_max_vol));

        // adjust for master volume
        const auto m_vol = m_mixer.getMasterVolume();
        m_volL = ((m_volL * m_vol) / mixer::Mixer_max_volume);
        m_volR = ((m_volR * m_vol) / mixer::Mixer_max_volume);
    }
}
