#pragma once

#include <cstdint>
#include <cstddef>

namespace HyperSonicDrivers::audio
{
    class IAudioStream
    {
    public:
        IAudioStream() = default;
        virtual ~IAudioStream() = default;

        virtual size_t readBuffer(int16_t* buffer, const size_t numSamples) = 0;

        virtual bool isStereo() const = 0;

        virtual int getRate() const = 0;

        virtual bool endOfData() const = 0;

        virtual bool isEnded() const { return endOfData(); }
    };
}
