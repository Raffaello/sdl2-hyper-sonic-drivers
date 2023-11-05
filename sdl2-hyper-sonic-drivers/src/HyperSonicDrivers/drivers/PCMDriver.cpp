#include <algorithm>
#include <HyperSonicDrivers/drivers/PCMDriver.hpp>

namespace HyperSonicDrivers::drivers
{
    using audio::streams::PCMStream;

    PCMDriver::PCMDriver(const std::shared_ptr<audio::IMixer>& mixer, const uint8_t max_channels) :
        max_streams(std::min(mixer->max_channels, max_channels)), m_mixer(mixer)
    {
    }

    bool PCMDriver::isPlaying() const noexcept
    {
        for (const auto& [stream, _] : m_PCMStreams_channels)
        {
            if (isPCMStreamPlaying_(stream))
                return true;
        }

        return false;
    }

    bool PCMDriver::isPlaying(const std::shared_ptr<audio::PCMSound>& sound) const noexcept
    {
        for (const auto& [stream, _] : m_PCMStreams_channels)
        {
            if (stream->getSound() == sound)
                return isPCMStreamPlaying_(stream);
        }

        return false;
    }

    std::optional<uint8_t> PCMDriver::play(const std::shared_ptr<audio::PCMSound>& sound, const uint8_t volume, const int8_t pan)
    {
        releaseEndedStreams_();
        if (m_PCMStreams_channels.size() == max_streams)
            return std::nullopt;

        auto s = std::make_shared<PCMStream>(sound);

        auto channelId =  m_mixer->play(
            sound->group,
            s,
            volume,
            pan
        );

        if (channelId.has_value())
            m_PCMStreams_channels[s] = channelId.value();

        return channelId;
    }

    void PCMDriver::stop(const uint8_t channel_id, const bool releaseEndedStreams) noexcept
    {
        auto it = std::ranges::find_if(
            m_PCMStreams_channels,
            [channel_id](std::pair<const std::shared_ptr<audio::streams::PCMStream>&, const int> p) {
                return channel_id == p.second;
            }
        );

        if (it == m_PCMStreams_channels.end())
            return;

        if (!(it->first)->isEnded())
            m_mixer->reset(channel_id);

        if (releaseEndedStreams)
        {
            m_PCMStreams_channels.erase(it);
            releaseEndedStreams_();
        }
    }

    void PCMDriver::stop(const std::shared_ptr<audio::PCMSound>& sound, const bool releaseEndedStreams)
    {
        auto it = std::ranges::find_if(
            m_PCMStreams_channels,
            [&sound](std::pair<const std::shared_ptr<audio::streams::PCMStream>&, const int> p) {
                return p.first != nullptr && sound == p.first->getSound();
            }
        );

        if (it == m_PCMStreams_channels.end())
            return;

        stop(it->second, releaseEndedStreams);
    }

    void PCMDriver::stop() noexcept
    {
        for (const auto& [_, ch_id] : m_PCMStreams_channels)
            stop(ch_id, false);

        releaseStreams_();
    }

    void PCMDriver::releaseEndedStreams_() noexcept
    {
        for (auto it = m_PCMStreams_channels.begin(); it != m_PCMStreams_channels.end();)
        {
            if (!isPCMStreamPlaying_(it->first))
                it = m_PCMStreams_channels.erase(it);
            else
                ++it;
        }
    }

    void PCMDriver::releaseStreams_() noexcept
    {
        m_PCMStreams_channels.clear();
    }

    inline bool PCMDriver::isPCMStreamPlaying_(const std::shared_ptr<audio::streams::PCMStream>& stream) noexcept
    {
        return stream != nullptr && !stream->isEnded();
    }
}
