#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <mutex>
#include <optional>
#include <HyperSonicDrivers/audio/mixer/MixerBase.hpp>
#include <HyperSonicDrivers/audio/mixer/Channel.hpp>

#include <SDL2/SDL_audio.h>

namespace HyperSonicDrivers::audio::sdl2
{
class Mixer : public mixer::MixerBase
{
public:
    Mixer(const uint8_t  max_channels,
          const uint32_t freq,
          const uint16_t buffer_size);
    ~Mixer() override = default;

    void suspend() noexcept override;
    void resume() noexcept override;

protected:
    bool onInit_() override;
    void onShutdown_() override;

private:
    bool init_(SDL_AudioCallback callback, void* userdata);

    size_t      callback(uint8_t* samples, unsigned int len);
    static void sdlCallback(void* this_, uint8_t* samples, int len);

    SDL_AudioDeviceID m_device_id = 0;
};

}    // namespace HyperSonicDrivers::audio::sdl2
