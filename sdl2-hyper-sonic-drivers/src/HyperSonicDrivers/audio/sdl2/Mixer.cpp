#include <algorithm>
#include <ranges>
#include <cstring>
#include <cassert>
#include <HyperSonicDrivers/audio/sdl2/Mixer.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>

#include <SDL2/SDL.h>

namespace HyperSonicDrivers::audio::sdl2
{
    using utils::logI;
    using utils::logW;
    using utils::logE;

    Mixer::Mixer(const uint8_t max_channels,
        const uint32_t freq, const uint16_t buffer_size) :
        IMixer(max_channels, freq, buffer_size)
    {
        m_channels.resize(max_channels);
        for (int i = 0; i < max_channels; i++)
        {
            m_channels[i] = std::make_unique<mixer::Channel>(*this, static_cast<uint8_t>(i));
        }
    }

    Mixer::~Mixer()
    {
        if (m_ready)
        {
            SDL_CloseAudioDevice(m_device_id);
            SDL_QuitSubSystem(SDL_INIT_AUDIO);
            m_ready = false;
        }
    }

    bool Mixer::init()
    {
        return init_(sdlCallback, this);
    }

    std::optional<uint8_t> Mixer::play(
        const mixer::eChannelGroup group,
        const std::shared_ptr<IAudioStream>& stream,
        const uint8_t vol, const int8_t pan
    ) {
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
        SDL_PauseAudioDevice(m_device_id, 1);
    }

    void Mixer::resume() noexcept
    {
        SDL_PauseAudioDevice(m_device_id, 0);
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

        return std::ranges::any_of(m_channels, [](const auto& ch)
            { return !ch->isEnded(); });
    }

    bool Mixer::isActive(const mixer::eChannelGroup group) const noexcept
    {
        std::scoped_lock lck(m_mutex);

        return std::ranges::any_of(m_channels, [group](const auto& ch)
            { return ch->getChannelGroup() == group && !ch->isEnded(); });

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

    bool Mixer::init_(SDL_AudioCallback callback, void* userdata)
    {
        m_ready = false;
        if (SDL_InitSubSystem(SDL_INIT_AUDIO) == -1)
        {
            logE("Can't initialize SDL Audio");
            return false;
        }

        // Get the desired audio specs
        SDL_AudioSpec desired = {
            .freq = static_cast<int>(freq),
            .format = AUDIO_S16,
            .channels = 2,
            .samples = buffer_size,
            .callback = callback,
            .userdata = userdata
        };

        SDL_AudioSpec obtained;
        m_device_id = SDL_OpenAudioDevice(nullptr, 0, &desired, &obtained, 0);
        if (m_device_id == 0)
        {
            logE("can't open audio device");
            return false;
        }

        const char* sdlDriverName = SDL_GetCurrentAudioDriver();
        logI(std::format("Using SDL Audio Driver '{}'", sdlDriverName));

        if (obtained.format != desired.format)
        {
            logW("format different");
        }
        if (obtained.freq != desired.freq)
        {
            logW(std::format("freq different: obtained={}, desired={}", obtained.freq, desired.freq));
        }
        if (obtained.channels != desired.channels)
        {
            logW(std::format("channels different: obtained={}, desired={}", obtained.channels, desired.channels));
        }

        resume();
        m_ready = true;

        return true;
    }

    size_t Mixer::callback(uint8_t* samples, unsigned int len)
    {
        const std::scoped_lock lck(m_mutex);

        int16_t* buf = std::bit_cast<int16_t*>(samples);
        // we store stereo, 16-bit samples (div 2 for stereo and 2 from 8 to 16 bits)
        assert(len % 4 == 0);
        //  zero the buf (size of 2ch stereo: len*2 of 16 bits)
        memset(buf, 0, len);
        len >>= 2;
        // mix all channels
        size_t res = 0;
        for (const auto& ch : m_channels)
        {
            const size_t tmp = ch->mix(buf, len);

            // TODO: returning a value can be removed
            if (tmp > res)
                res = tmp;
        }

        return res;
    }

    void Mixer::sdlCallback(void* userdata, uint8_t* stream, int len)
    {
        Mixer* mixer = reinterpret_cast<Mixer*>(userdata);
        assert(mixer != nullptr);
        mixer->callback(stream, len);
    }
}
