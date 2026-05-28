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
             const uint16_t buffer_size) : MixerBase(max_channels, freq, buffer_size)
{
}

Mixer::~Mixer()
{
    shutdown();
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

bool Mixer::onInit_()
{
    return init_(sdlCallback_, this);
}

void Mixer::onShutdown_()
{
    if (m_pStream != nullptr)
    {
        SDL_DestroyAudioStream(m_pStream);
        m_pStream = nullptr;
    }

    if (m_device_id != 0)
    {
        SDL_CloseAudioDevice(m_device_id);
        m_device_id = 0;
    }

    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

bool Mixer::init_(SDL_AudioStreamCallback callback, void* userdata)
{
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

    return true;
}

void Mixer::callback_(SDL_AudioStream* stream, int additional_amount, int total_amount)
{
    const std::scoped_lock lck(m_mutex);

    // we store stereo, 16-bit samples (div 2 for stereo and 2 from 8 to 16 bits)
    const int num_samples = additional_amount / sizeof(int16_t);
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
