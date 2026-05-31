#include <HyperSonicDrivers/audio/mixer/MixerBase.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>

#include <cassert>

namespace HyperSonicDrivers::audio::mixer
{

using HyperSonicDrivers::utils::logW;
using HyperSonicDrivers::utils::logE;

MixerBase::MixerBase(const uint8_t  max_channels,
                     const uint32_t freq,
                     const uint16_t buffer_size) : IMixer(max_channels, freq, buffer_size)
{
    m_channels.resize(max_channels);
    for (int i = 0; i < max_channels; i++)
    {
        m_channels[i] = std::make_unique<mixer::Channel>(*this, static_cast<uint8_t>(i));
    }
}

bool MixerBase::init()
{
    m_ready = false;
    if (onInit_())
    {
        resume();
        m_ready = true;
    }

    return isReady();
}

void MixerBase::shutdown()
{
    if (isReady())
    {
        m_ready = false;
        suspend();
        onShutdown_();
    }
}

std::optional<uint8_t> MixerBase::play(
    const mixer::eChannelGroup           group,
    const std::shared_ptr<IAudioStream>& stream,
    const uint8_t                        vol,
    const int8_t                         pan)
{
    std::lock_guard lock(m_mutex);
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
        return std::nullopt;
    }

    m_channels[i]->setAudioStream(group, stream, vol, pan, m_reverseStereo);
    return i;
}

void MixerBase::reset() noexcept
{
    std::scoped_lock lck(m_mutex);

    for (const auto& ch : m_channels)
        ch->reset();
}

void MixerBase::reset(const uint8_t id) noexcept
{
    std::scoped_lock lck(m_mutex);
    assert(id < m_channels.size());
    m_channels[id]->reset();
}

void MixerBase::reset(const mixer::eChannelGroup group) noexcept
{
    std::scoped_lock lck(m_mutex);

    for (const auto& ch : m_channels)
    {
        if (ch->getChannelGroup() == group)
            ch->reset();
    }
}

void MixerBase::pause() noexcept
{
    std::scoped_lock lck(m_mutex);

    for (const auto& ch : m_channels)
        ch->pause();
}

void MixerBase::pause(const uint8_t id) noexcept
{
    std::scoped_lock lck(m_mutex);

    m_channels[id]->pause();
}

void MixerBase::unpause() noexcept
{
    std::scoped_lock lck(m_mutex);

    for (const auto& ch : m_channels)
        ch->unpause();
}

void MixerBase::unpause(const uint8_t id) noexcept
{
    std::scoped_lock lck(m_mutex);

    m_channels[id]->unpause();
}

bool MixerBase::isActive(const uint8_t id) const noexcept
{
    std::scoped_lock lck(m_mutex);

    return !m_channels[id]->isEnded();
}

bool MixerBase::isPaused(const uint8_t id) const noexcept
{
    std::scoped_lock lck(m_mutex);

    return m_channels[id]->isPaused();
}

bool MixerBase::isActive() const noexcept
{
    std::scoped_lock lck(m_mutex);

    return std::ranges::any_of(m_channels, [](const auto& ch) { return !ch->isEnded(); });
}

bool MixerBase::isActive(const mixer::eChannelGroup group) const noexcept
{
    std::scoped_lock lck(m_mutex);

    return std::ranges::any_of(m_channels, [group](const auto& ch) { return ch->getChannelGroup() == group && !ch->isEnded(); });
}

bool MixerBase::isChannelGroupMuted(const mixer::eChannelGroup group) const noexcept
{
    assert(group2i(group) < m_group_settings.size());
    return m_group_settings[group2i(group)].mute;
}

void MixerBase::muteChannelGroup(const mixer::eChannelGroup group) noexcept
{
    std::scoped_lock lck(m_mutex);

    m_group_settings[group2i(group)].mute = true;
    updateChannelsVolumePan_();
}

void MixerBase::unmuteChannelGroup(const mixer::eChannelGroup group) noexcept
{
    std::scoped_lock lck(m_mutex);

    m_group_settings[group2i(group)].mute = false;
    updateChannelsVolumePan_();
}

uint8_t MixerBase::getChannelVolume(const uint8_t id) const noexcept
{
    return m_channels[id]->getVolume();
}

void MixerBase::setChannelVolume(const uint8_t id, const uint8_t volume) noexcept
{
    std::scoped_lock lck(m_mutex);

    m_channels[id]->setVolume(volume);
}

uint8_t MixerBase::getChannelPan(const uint8_t id) const noexcept
{
    return m_channels[id]->getPan();
}

void MixerBase::setChannelPan(const uint8_t id, const int8_t pan) noexcept
{
    std::scoped_lock lck(m_mutex);

    m_channels[id]->setPan(pan);
}

void MixerBase::setChannelVolumePan(const uint8_t id, const uint8_t volume, const int8_t pan) noexcept
{
    std::scoped_lock lck(m_mutex);

    m_channels[id]->setVolumePan(volume, pan);
}

mixer::eChannelGroup MixerBase::getChannelGroup(const uint8_t id) const noexcept
{
    return m_channels[id]->getChannelGroup();
}

void MixerBase::setMasterVolume(const uint8_t master_volume) noexcept
{
    std::scoped_lock lck(m_mutex);

    m_master_volume = master_volume;
    updateChannelsVolumePan_();
}

void MixerBase::updateChannelsVolumePan_() noexcept
{
    for (const auto& ch : m_channels)
        ch->updateVolumePan();
}

}    // namespace HyperSonicDrivers::audio::mixer
