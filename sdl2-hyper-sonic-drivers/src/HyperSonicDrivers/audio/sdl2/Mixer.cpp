#include <algorithm>
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
        SDL_CloseAudioDevice(m_device_id);

        SDL_QuitSubSystem(SDL_INIT_AUDIO);
    }

    bool Mixer::init()
    {
        m_ready = false;
        if (SDL_InitSubSystem(SDL_INIT_AUDIO) == -1)
        {
            logE("Can't initialize SDL Audio");
            return false;
        }

        const char* sdlDriverName = SDL_GetCurrentAudioDriver();
        logI(std::format("Using SDL Audio Driver '{}'", sdlDriverName));

        // Get the desired audio specs
        SDL_AudioSpec desired = {
            .freq = static_cast<int>(m_sampleRate),
            .format = AUDIO_S16,
            .channels = 2,
            .samples = m_samples,
            .callback = sdlCallback,
            .userdata = this
        };

        SDL_AudioSpec obtained;
        m_device_id = SDL_OpenAudioDevice(nullptr, 0, &desired, &obtained, 0);
        if (m_device_id == 0)
        {
            logE("can't open audio device");
            return false;
        }

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

        SDL_PauseAudioDevice(m_device_id, 0);
        m_ready = true;

        return true;
    }

    std::optional<uint8_t> Mixer::play(
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
            return std::nullopt;
        }

        m_channels[i]->setAudioStream(group, stream, vol, pan, reverseStereo);

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

        for (auto& ch : m_channels)
            ch->reset();
    }

    void Mixer::reset(const uint8_t id) noexcept
    {
        std::scoped_lock lck(m_mutex);

        m_channels[id]->reset();
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

    bool Mixer::isPaused(const uint8_t id) const noexcept
    {
        std::scoped_lock lck(m_mutex);

        return m_channels[id]->isPaused();
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
        std::scoped_lock lck(m_mutex);

        for (auto& ch : m_channels)
            ch->updateVolumePan();
    }

    size_t Mixer::callback(uint8_t* samples, unsigned int len)
    {
        const std::scoped_lock lck(m_mutex);

        int16_t* buf = reinterpret_cast<int16_t*>(samples);
        // we store stereo, 16-bit samples (2 for stereo, 2 from 8 to 16 bits)
        assert(len % 4 == 0);
        len >>= 2;

        //  zero the buf
        memset(buf, 0, 2 * len * sizeof(int16_t));

        // mix all channels
        size_t res = 0;
        for (auto& ch : m_channels)
        {
            const size_t tmp = ch->mix(buf, len);

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
