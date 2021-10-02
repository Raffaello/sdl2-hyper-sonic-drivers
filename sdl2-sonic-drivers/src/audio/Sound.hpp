#pragma once

#include <audio/scummvm/AudioStream.hpp>
#include <audio/scummvm/Mixer.hpp>
#include <cstdint>
#include <memory>

namespace audio
{
    class Sound final: public scummvm::AudioStream
    {
    public:
        Sound(const Sound&) = delete;
        Sound(const scummvm::Mixer::SoundType soundType, const bool isStereo, const int rate, const uint8_t bitsDepth, const uint32_t dataSize, const std::shared_ptr<uint8_t[]> data);

        virtual int readBuffer(int16_t* buffer, const int numSamples);
        virtual bool isStereo() const;
        virtual int getRate() const;
        virtual bool endOfData() const;

        uint8_t getBitsDepth() const noexcept;
        const scummvm::Mixer::SoundType getSoundType() const noexcept;
    private:
        uint32_t _dataSize;
        std::shared_ptr<uint8_t[]> _data;
        bool _stereo;
        int _rate;
        uint8_t _bitsDepth;

        int _bitsFactor;

        scummvm::Mixer::SoundType _soundType;
    };
}
