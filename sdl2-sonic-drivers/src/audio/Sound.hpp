#pragma once

#include <audio/scummvm/AudioStream.hpp>
#include <audio/scummvm/SoundHandle.hpp>
#include <cstdint>
#include <memory>

namespace audio
{
    class Sound final: public scummvm::AudioStream
    {
    public:
        Sound(const Sound&) = delete;
        Sound(const bool isStereo, const int rate, const uint8_t bitsDepth, const uint32_t dataSize, const std::shared_ptr<uint8_t[]> data);

        virtual int readBuffer(int16_t* buffer, const int numSamples);
        virtual bool isStereo() const;
        virtual int getRate() const;
        virtual bool endOfData() const;

        uint8_t getBitsDepth() const noexcept;

    private:
        uint32_t _dataSize;
        std::shared_ptr<uint8_t[]> _data;
        bool _stereo;
        int _rate;
        uint8_t _bitsDepth;
        audio::scummvm::SoundHandle* _handle;

        uint32_t _curPos;
        int _bitsFactor;
    };
}
