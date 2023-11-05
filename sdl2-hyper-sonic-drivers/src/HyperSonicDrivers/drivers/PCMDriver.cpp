#include <algorithm>
#include <HyperSonicDrivers/drivers/PCMDriver.hpp>

namespace HyperSonicDrivers::drivers
{
    using audio::streams::PCMStream;

    PCMDriver::PCMDriver(const std::shared_ptr<audio::IMixer>& mixer, const uint8_t max_channels) :
        max_streams(std::min(mixer->max_channels, max_channels)), m_mixer(mixer)
    {
        m_PCMStreams.resize(max_streams);
    }

    bool PCMDriver::isPlaying() const noexcept
    {
        for(const auto& ss: m_PCMStreams)
        {
            if (isPCMStreamPlaying_(ss))
                return true;
        }

        return false;
    }

    bool PCMDriver::isPlaying(const std::shared_ptr<audio::PCMSound>& sound) const noexcept
    {
        // TODO:
        // should map channelId to check directly in the mixer?
        // how to find a free slot then? 
        // does we need to really track it?
        // probably using a map instead of a vector is ok,
        // no need to define nether max-channels.
        // but that is because if wanting to reserve some channels for something
        // else that is not PCM related...
        // anyway... it could be achieved having the mixer a "lock or reserved channel"
        // feature or something that that one won't be used unless
        // it is for the resources that has been reserved for.....
        for(const auto& ss : m_PCMStreams)
        {
            if (ss->getSound() == sound)
                return isPCMStreamPlaying_(ss);
        }

        /*for (const auto& ss : m_PCMStreams_channels)
        {
            if (ss.first->getSound() == sound)
                return isPCMStreamPlaying_(ss.first);
        }*/

        return false;
    }

    std::optional<uint8_t> PCMDriver::play(const std::shared_ptr<audio::PCMSound>& sound, const uint8_t volume, const int8_t pan)
    {
        // find first free slot
        auto it = std::ranges::find_if_not(m_PCMStreams, isPCMStreamPlaying_);
        if (it == m_PCMStreams.end())
            return std::nullopt;

        //releaseEndedStreams_();
        //if (m_PCMStreams_channels.size() == max_streams)
        //    return std::nullopt;

        *it = std::make_shared<PCMStream>(sound);

        auto channelId =  m_mixer->play(
            sound->group,
            *it,
            volume,
            pan
        );

        if (!channelId.has_value())
            *it = nullptr;
        else
            m_PCMStreams_channels[*it] = channelId.value();

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

        m_PCMStreams_channels.erase(it);
        if (releaseEndedStreams)
            releaseEndedStreams_();
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
        for (int i = 0; i < m_PCMStreams.size(); i++)
        {
            stop(m_PCMStreams[i]->getSound(), false);
        }

        releaseEndedStreams_();
    }

    void PCMDriver::releaseEndedStreams_() noexcept
    {
        for (int i = 0; i < m_PCMStreams.size(); i++)
        {
            if (!isPCMStreamPlaying_(m_PCMStreams[i]))
            {
                if (m_PCMStreams_channels.contains(m_PCMStreams[i]))
                    m_PCMStreams_channels.erase(m_PCMStreams[i]);

                m_PCMStreams[i] = nullptr;
            }
        }
        //assert(m_PCMStreams_channels.size() == 0);
    }

    inline bool PCMDriver::isPCMStreamPlaying_(const std::shared_ptr<audio::streams::PCMStream>& stream) noexcept
    {
        return stream != nullptr && !stream->isEnded();
    }
}
