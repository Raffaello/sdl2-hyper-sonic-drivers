#include <HyperSonicDrivers/audio/sdl2/Mixer.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>

#include <SDL2/SDL.h>    // due to init/quit subsystem

#include <algorithm>
#include <ranges>
#include <cstring>
#include <cassert>
#include <bit>

namespace HyperSonicDrivers::audio::sdl2
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
    SDL_PauseAudioDevice(m_device_id, 1);
}

void Mixer::resume() noexcept
{
    SDL_PauseAudioDevice(m_device_id, 0);
}

bool Mixer::onInit_()
{
    return init_(sdlCallback, this);
}

void Mixer::onShutdown_()
{
    if (m_device_id != 0)
    {
        SDL_CloseAudioDevice(m_device_id);
        m_device_id = 0;
    }

    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

bool Mixer::init_(SDL_AudioCallback callback, void* userdata)
{
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) == -1)
    {
        logE("Can't initialize SDL Audio");
        return false;
    }

    // Get the desired audio specs
    SDL_AudioSpec desired{};

    desired.freq     = static_cast<int>(freq);
    desired.format   = AUDIO_S16;
    desired.channels = 2;
    desired.samples  = buffer_size;
    desired.callback = callback;
    desired.userdata = userdata;

    SDL_AudioSpec obtained{};
    m_device_id = SDL_OpenAudioDevice(nullptr, 0, &desired, &obtained, 0);
    if (m_device_id == 0)
    {
        logE("can't open audio device");
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
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

    return true;
}

size_t Mixer::callback(uint8_t* samples, unsigned int len)
{
    const std::scoped_lock lck(m_mutex);

    int16_t* buf = std::bit_cast<int16_t*>(samples);
    // we store stereo, 16-bit samples (div 2 for stereo and 2 from 8 to 16 bits)
    assert(len % 4 == 0);
    // zero the buf (size of 2ch stereo: len*2 of 16 bits)
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
}    // namespace HyperSonicDrivers::audio::sdl2
