#pragma once

#include <cstdint>
#include <cstddef>

namespace HyperSonicDrivers::audio
{
    /**
    * This stream must be same bits of the mixer output
    **/
    class IAudioStream
    {
    public:
        IAudioStream(IAudioStream&) = delete;
        IAudioStream(IAudioStream&&) = delete;
        IAudioStream& operator=(IAudioStream&) = delete;

        IAudioStream() = default;
        virtual ~IAudioStream() = default;

        virtual size_t readBuffer(int16_t* buffer, const size_t numSamples) = 0;
        virtual bool isStereo() const = 0;
        virtual uint32_t getRate() const = 0;
        virtual bool endOfData() const = 0;
        virtual bool isEnded() const { return endOfData(); }
    };
}
