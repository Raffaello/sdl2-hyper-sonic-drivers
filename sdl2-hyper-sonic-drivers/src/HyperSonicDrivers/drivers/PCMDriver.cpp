#include <HyperSonicDrivers/drivers/PCMDriver.hpp>
#include <algorithm>

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
        for(const auto& ss: m_soundStreams)
        {
            if (isSoundStreamPlaying_(ss))
                return true;
        }

        return false;
    }

    bool PCMDriver::isPlaying(const std::shared_ptr<audio::Sound>& sound) const noexcept
    {
        // TODO: should be returned the soundHandle or soundID in play method to be used later on?
        // BODY: so here it can be addressed in constant time instead of searching for sound in the slots?
        for(const auto& ss : m_soundStreams)
        {
            if (ss->getSound().lock() == sound)
                return isSoundStreamPlaying_(ss);
        }

        return false;
    }

    std::optional<uint8_t> PCMDriver::play(const std::shared_ptr<audio::Sound>& sound, const uint8_t volume, const int8_t pan, const bool reverseStereo)
    {
        // find first free slot
        auto it = std::ranges::find_if_not(m_soundStreams, isSoundStreamPlaying_);
        if (it == m_soundStreams.end())
            return std::nullopt;

        *it = std::make_shared<SoundStream>(sound);

        return m_mixer->play(
            sound->group,
            *it,
            volume,
            pan,
            reverseStereo
        );
    }

    inline bool PCMDriver::isSoundStreamPlaying_(const std::shared_ptr<audio::streams::SoundStream>& ss) noexcept
    {
        return ss != nullptr && !ss->isEnded();
    }
}
