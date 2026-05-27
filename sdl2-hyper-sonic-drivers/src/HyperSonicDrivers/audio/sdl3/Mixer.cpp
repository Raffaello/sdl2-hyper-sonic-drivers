#include <HyperSonicDrivers/audio/sdl3/Mixer.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>

#include <SDL3/SDL.h>    // due to init/quit subsystem

#include <algorithm>
#include <ranges>
#include <cstring>
#include <cassert>
#include <bit>

namespace HyperSonicDrivers::audio::sdl3
{
using utils::logE;
using utils::logI;
using utils::logW;

Mixer::Mixer(const uint8_t  max_channels,
             const uint32_t freq,
             const uint16_t buffer_size) : IMixer(max_channels, freq, buffer_size)
{
    m_channels.resize(max_channels);
    for (int i = 0; i < max_channels; i++)
    {
        m_channels[i] = std::make_unique<mixer::Channel>(*this, static_cast<uint8_t>(i));
    }
}

Mixer::~Mixer()
{
    shutdown();
}

bool Mixer::init()
{
    return init_(sdlCallback_, this);
}

void Mixer::shutdown()
{
    if (isReady())
    {
        suspend();
        SDL_CloseAudioDevice(m_device_id);
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        m_ready     = false;
        m_device_id = 0;
    }
}

std::optional<uint8_t> Mixer::play(
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

    return std::make_optional(static_cast<uint8_t>(i));
}

void Mixer::suspend() noexcept
{
    if (!SDL_PauseAudioDevice(m_device_id))
    {
        logE(std::format("unable to pause audio device id: {} [{}]", m_device_id, SDL_GetError()));
        SDL_ClearError();
    }
}

void Mixer::resume() noexcept
{
    if (!SDL_ResumeAudioDevice(m_device_id))
    {
        logE(std::format("unable to resume audio device id: {} [{}]", m_device_id, SDL_GetError()));
        SDL_ClearError();
    }
}

void Mixer::reset() noexcept
{
    std::scoped_lock lck(m_mutex);

    for (const auto& ch : m_channels)
        ch->reset();
}

void Mixer::reset(const uint8_t id) noexcept
{
    std::scoped_lock lck(m_mutex);

    m_channels[id]->reset();
}

void Mixer::reset(const mixer::eChannelGroup group) noexcept
{
    std::scoped_lock lck(m_mutex);

    for (const auto& ch : m_channels)
    {
        if (ch->getChannelGroup() == group)
            ch->reset();
    }
}

void Mixer::pause() noexcept
{
    std::scoped_lock lck(m_mutex);

    for (const auto& ch : m_channels)
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

    for (const auto& ch : m_channels)
        ch->unpause();
}

void Mixer::unpause(const uint8_t id) noexcept
{
    std::scoped_lock lck(m_mutex);

    m_channels[id]->unpause();
}

bool Mixer::isActive(const uint8_t id) const noexcept
{
    std::scoped_lock lck(m_mutex);

    return !m_channels[id]->isEnded();
}

bool Mixer::isPaused(const uint8_t id) const noexcept
{
    std::scoped_lock lck(m_mutex);

    return m_channels[id]->isPaused();
}

bool Mixer::isActive() const noexcept
{
    std::scoped_lock lck(m_mutex);

    return std::ranges::any_of(m_channels, [](const auto& ch) { return !ch->isEnded(); });
}

bool Mixer::isActive(const mixer::eChannelGroup group) const noexcept
{
    std::scoped_lock lck(m_mutex);

    return std::ranges::any_of(m_channels, [group](const auto& ch) { return ch->getChannelGroup() == group && !ch->isEnded(); });

    return false;
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

mixer::eChannelGroup Mixer::getChannelGroup(const uint8_t id) const noexcept
{
    return m_channels[id]->getChannelGroup();
}

void Mixer::setMasterVolume(const uint8_t master_volume) noexcept
{
    std::scoped_lock lck(m_mutex);

    m_master_volume = master_volume;
    updateChannelsVolumePan_();
}

void Mixer::updateChannelsVolumePan_() noexcept
{
    for (const auto& ch : m_channels)
        ch->updateVolumePan();
}

bool Mixer::init_(SDL_AudioStreamCallback callback, void* userdata)
{
    m_ready = false;
    if (!SDL_InitSubSystem(SDL_INIT_AUDIO))
    {
        logE("Can't initialize SDL3 Audio");
        return false;
    }

    // Get the desired audio specs
    SDL_AudioSpec spec{
        .format   = SDL_AUDIO_S16,
        .channels = 2,
        .freq     = static_cast<int>(freq),
    };

    m_pStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, callback, userdata);
    if (m_pStream == nullptr)
    {
        logE(std::format("'can't open audio stream: {}", SDL_GetError()));
        SDL_ClearError();
        return false;
    }

    m_device_id = SDL_GetAudioStreamDevice(m_pStream);

    SDL_AudioSpec obtained{};
    if (!SDL_GetAudioStreamFormat(m_pStream, nullptr, &obtained))
    {
        logE("unable to get audio stream format");
        return false;
    }

    const char* sdlDriverName = SDL_GetCurrentAudioDriver();
    logI(std::format("Using SDL Audio Driver '{}'", sdlDriverName));

    if (spec.format != obtained.format)
        logW("format different");
    if (spec.freq != obtained.freq)
        logW(std::format("freq different: obtained={}, desired={}", spec.freq, obtained.freq));
    if (spec.channels != obtained.channels)
    {
        logW(std::format("channels different: obtained={}, desired={}", spec.channels, obtained.channels));
    }

    resume();
    m_ready = true;

    return true;
}

void Mixer::callback_(SDL_AudioStream* stream, int additional_amount, int total_amount)
{
    const std::scoped_lock lck(m_mutex);

    // we store stereo, 16-bit samples (div 2 for stereo and 2 from 8 to 16 bits)
    const int num_samples = total_amount / sizeof(int16_t);
    // assert(num_samples % 2 == 0);
    int16_t* buf = SDL_stack_alloc(int16_t, num_samples);
    // zero the buf (size of 2ch stereo: len*2 of 16 bits)
    memset(buf, 0, total_amount);

    // mix all channels
    for (const auto& ch : m_channels)
        ch->mix(buf, num_samples / 2);    // stereo

    SDL_PutAudioStreamData(stream, buf, total_amount);
    SDL_stack_free(buf);
}

void Mixer::sdlCallback_(void* userdata, SDL_AudioStream* stream, int additional_amount, int total_amount)
{
    Mixer* mixer = reinterpret_cast<Mixer*>(userdata);
    assert(mixer != nullptr);

    mixer->callback_(stream, additional_amount, total_amount);
}
}    // namespace HyperSonicDrivers::audio::sdl3
