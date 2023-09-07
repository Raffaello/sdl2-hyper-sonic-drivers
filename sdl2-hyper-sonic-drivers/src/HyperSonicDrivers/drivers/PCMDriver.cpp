#include <HyperSonicDrivers/drivers/PCMDriver.hpp>

namespace HyperSonicDrivers::drivers
{
    using audio::streams::SoundStream;

    PCMDriver::PCMDriver(const std::shared_ptr<audio::IMixer>& mixer, const uint8_t max_channels) :
        m_mixer(mixer)
    {
        m_max_streams = std::min(m_mixer->max_channels, max_channels);
        m_soundStreams.resize(m_max_streams);
    }

    bool PCMDriver::isPlaying() const noexcept
    {
        for (int i = 0; i < m_max_streams; ++i)
        {
            const auto& ss = m_soundStreams[i];
            if(ss != nullptr && !ss->isEnded())
                return true;
        }

        return false;
    }

    bool PCMDriver::isPlaying(const std::shared_ptr<audio::Sound>& sound) const noexcept
    {
        // TODO: should be returned the soundHandle or soundID in play method to be used later on?
        // BODY: so here it can be addressed in constant time instead of searching for sound in the slots?
        for (int i = 0; i < m_max_streams; i++)
        {
            const auto& ss = m_soundStreams[i];
            if (ss != nullptr
                && ss->getSound().lock() == sound
                && !ss->isEnded()
                )
                return true;
        }

        return false;
    }

    std::optional<uint8_t> PCMDriver::play(const std::shared_ptr<audio::Sound>& sound, const uint8_t volume, const int8_t pan, const bool reverseStereo)
    {
        // TODO: this method is not thread-safe at the moment.
        int cur_stream;
        
        // find first free slot
        for (cur_stream = 0; cur_stream < m_max_streams ; ++cur_stream)
        {
            const auto& ss = m_soundStreams[cur_stream];
            if (ss == nullptr || ss->isEnded())
                break;
        }

        if (cur_stream == m_max_streams)
            return std::nullopt;

        m_soundStreams[cur_stream] = std::make_shared<SoundStream>(sound);

        return m_mixer->play(
            sound->group,
            m_soundStreams[cur_stream],
            volume,
            pan,
            reverseStereo
        );
    }
}
