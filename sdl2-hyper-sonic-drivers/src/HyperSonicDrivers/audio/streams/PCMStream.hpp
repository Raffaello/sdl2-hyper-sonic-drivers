#pragma once

#include <cstdint>
#include <memory>
#include <HyperSonicDrivers/audio/PCMSound.hpp>
#include <HyperSonicDrivers/audio/IAudioStream.hpp>

namespace HyperSonicDrivers::audio::streams
{
    class PCMStream final : public IAudioStream
    {
    public:
        explicit PCMStream(const std::shared_ptr<PCMSound>& sound);
        ~PCMStream() override = default;

        size_t readBuffer(int16_t* buffer, const size_t numSamples) override;
        bool isStereo() const override;
        uint32_t getRate() const override;
        bool endOfData() const override;

        std::shared_ptr<PCMSound>  getSound() const noexcept;
    private:
        std::shared_ptr<PCMSound> m_sound;
        uint32_t m_curPos = 0;
    };
}
