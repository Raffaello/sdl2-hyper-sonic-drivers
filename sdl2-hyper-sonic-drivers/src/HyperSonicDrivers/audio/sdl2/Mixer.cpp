#include <algorithm>
#include <HyperSonicDrivers/audio/sdl2/Mixer.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>

namespace HyperSonicDrivers::audio::sdl2
{
    using utils::logW;

    Mixer::Mixer(const uint8_t max_channels) : IMixer(max_channels)
    {
        m_channels.resize(max_channels);
        for (int i = 0; i < max_channels; i++)
        {
            m_channels[i] = std::make_unique<mixer::Channel>(*this, static_cast<uint8_t>(i));
        }
    }

    bool Mixer::init()
    {
        return false;
    }

    void Mixer::play(
        const mixer::eChannelGroup group,
        const std::shared_ptr<IAudioStream>& stream,
        const uint8_t vol, const int8_t pan, const bool reverseStereo)
    {
        // find a free channel
        int i = 0;
        for (; i < max_channels; i++)
        {
            if (m_channels[i]->isEnded())
                break;
        }
        if (i == max_channels)
        {
            logW("no channels available. can't play");
            return;
        }

        m_channels[i]->setAudioStream(group, stream, vol, pan, reverseStereo);
    }

    void Mixer::stop() noexcept
    {
        std::scoped_lock lck(m_mutex);

        for (auto& ch : m_channels)
            ch->stop();
    }

    void Mixer::stop(const uint8_t id) noexcept
    {
        std::scoped_lock lck(m_mutex);

        m_channels[id]->stop();
    }

    void Mixer::pause() noexcept
    {
        std::scoped_lock lck(m_mutex);

        for (auto& ch : m_channels)
            ch->pause();
    }

    void Mixer::pause(const uint8_t id) noexcept
    {
        std::scoped_lock lck(m_mutex);

        m_channels[id]->pause();
    }

    void Mixer::unpause() noexcept
    {
        std::scoped_lock lck(m_mutex);

        for (auto& ch : m_channels)
            ch->unpause();
    }

    void Mixer::unpause(const uint8_t id) noexcept
    {
        std::scoped_lock lck(m_mutex);

        m_channels[id]->unpause();
    }

    bool Mixer::isChannelActive(const uint8_t id) const noexcept
    {
        std::scoped_lock lck(m_mutex);

        return !m_channels[id]->isEnded();
    }

    bool Mixer::isChannelGroupMuted(const mixer::eChannelGroup group) const noexcept
    {
        return m_group_settings[group2i(group)].mute;
    }

    void Mixer::muteChannelGroup(const mixer::eChannelGroup group) noexcept
    {
        std::scoped_lock lck(m_mutex);

        m_group_settings[group2i(group)].mute = true;
        updateChannelsVolumePan_();
    }

    void Mixer::unmuteChannelGroup(const mixer::eChannelGroup group) noexcept
    {
        std::scoped_lock lck(m_mutex);

        m_group_settings[group2i(group)].mute = false;
        updateChannelsVolumePan_();
    }

    uint8_t Mixer::getChannelVolume(const uint8_t id) const noexcept
    {
        return m_channels[id]->getVolume();
    }

    void Mixer::setChannelVolume(const uint8_t id, const uint8_t volume) noexcept
    {
        std::scoped_lock lck(m_mutex);

        m_channels[id]->setVolume(volume);
    }

    uint8_t Mixer::getChannelPan(const uint8_t id) const noexcept
    {
        return m_channels[id]->getPan();
    }

    void Mixer::setChannelPan(const uint8_t id, const int8_t pan) noexcept
    {
        std::scoped_lock lck(m_mutex);

        m_channels[id]->setPan(pan);
    }

    void Mixer::setChannelVolumePan(const uint8_t id, const uint8_t volume, const int8_t pan) noexcept
    {
        std::scoped_lock lck(m_mutex);

        m_channels[id]->setVolumePan(volume, pan);
    }

    void Mixer::updateChannelsVolumePan_() noexcept
    {
        for (auto& ch : m_channels)
            ch->updateVolumePan();
    }
}
