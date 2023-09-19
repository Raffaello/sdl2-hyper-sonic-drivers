#pragma once

#include <cstdint>
#include <cstddef>
#include <HyperSonicDrivers/audio/IAudioStream.hpp>
#include <HyperSonicDrivers/hardware/opl/EmulatedOPL.hpp>

namespace HyperSonicDrivers::audio::streams
{
    class OplStream : public audio::IAudioStream
    {
    private:
        hardware::opl::EmulatedOPL* m_opl = nullptr;
        uint32_t m_nextTick = 0;
    public:
        const bool stereo;
        const uint32_t rate;
        const uint32_t m_samplesPerTick;

        OplStream(
            hardware::opl::EmulatedOPL* opl,
            const bool stereo, const uint32_t rate, const uint32_t samplesPerTick);
        ~OplStream() override = default;

        size_t readBuffer(int16_t* buffer, const size_t numSamples) override;
        inline bool isStereo() const noexcept override { return stereo; }
        uint32_t getRate() const noexcept override { return rate; };
        bool endOfData() const noexcept override { return false; };
    };
}
