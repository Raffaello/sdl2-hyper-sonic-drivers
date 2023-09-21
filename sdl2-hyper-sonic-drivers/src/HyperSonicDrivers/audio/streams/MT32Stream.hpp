#pragma once

#include <cstdint>
#include <HyperSonicDrivers/audio/IAudioStream.hpp>
#include <HyperSonicDrivers/hardware/mt32/MT32.hpp>

namespace HyperSonicDrivers::audio::streams
{
    // TODO: this is the same of OplStream but instead of having an opl has MT32
    // TODO: create a "emulated stream" or something storing the parent class IHardware
    class MT32Stream : public IAudioStream
    {
    private:
        hardware::mt32::MT32* m_mt32 = nullptr;
        uint32_t m_nextTick = 0;

    public:
        const bool stereo;
        const uint32_t rate;
        const uint32_t samplesPerTick;

        MT32Stream(
            hardware::mt32::MT32* mt32,
            const bool stereo, const uint32_t rate, const uint32_t samplesPerTick);
        ~MT32Stream() override = default;

        size_t readBuffer(int16_t* buffer, const size_t numSamples) override;
        inline bool isStereo() const noexcept override { return stereo; }
        uint32_t getRate() const noexcept override { return rate; };
        bool endOfData() const noexcept override { return false; };
    };
}
