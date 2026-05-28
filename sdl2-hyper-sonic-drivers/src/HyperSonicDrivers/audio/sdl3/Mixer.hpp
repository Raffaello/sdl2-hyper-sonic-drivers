#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <mutex>
#include <optional>

#include <HyperSonicDrivers/audio/mixer/MixerBase.hpp>
#include <HyperSonicDrivers/audio/mixer/Channel.hpp>

#include <SDL3/SDL_audio.h>

namespace HyperSonicDrivers::audio::sdl3
{
class Mixer : public mixer::MixerBase
{
public:
    Mixer(const uint8_t  max_channels,
          const uint32_t freq,
          const uint16_t buffer_size);

    ~Mixer() override;

    // void shutdown() override;
    void suspend() noexcept override;
    void resume() noexcept override;

protected:
    bool onInit_() override;
    void onShutdown_() override;

private:
    bool init_(SDL_AudioStreamCallback callback, void* userdata);

    void        callback_(SDL_AudioStream* stream, int additional_amount, int total_amount);
    static void sdlCallback_(void* userdata, SDL_AudioStream* stream, int additional_amount, int total_amount);

    SDL_AudioDeviceID m_device_id = 0;
    SDL_AudioStream*  m_pStream;
};
}    // namespace HyperSonicDrivers::audio::sdl3
