#pragma once

#include <HyperSonicDrivers/audio/mixer/MixerBase.hpp>

#include <RtAudio.h>

namespace HyperSonicDrivers::audio::rtaudio
{
class Mixer : public mixer::MixerBase
{
public:
    Mixer(const uint8_t  max_channels,
          const uint32_t freq,
          const uint16_t buffer_size);

    ~Mixer() override;

    void suspend() noexcept override;
    void resume() noexcept override;

protected:
    bool onInit_() override;
    void onShutdown_() override;

private:
    bool init_(RtAudioCallback callback, void* userdata);

    void       callback_(int16_t* buf, int num_samples);
    static int rtAudioCallback_(void*               outputBuffer,
                                void*               inputBuffer,
                                unsigned int        nFrames,
                                double              streamTime,
                                RtAudioStreamStatus status,
                                void*               userData);

    RtAudio m_audio;
};

}    // namespace HyperSonicDrivers::audio::rtaudio
