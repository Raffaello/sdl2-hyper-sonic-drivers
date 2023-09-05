#pragma once

#include <cstdint>
#include <memory>
#include <HyperSonicDrivers/audio/Sound.hpp>
#include <HyperSonicDrivers/audio/IAudiotStream.hpp>
#include <HyperSonicDrivers/audio/scummvm/SoundHandle.hpp>

namespace HyperSonicDrivers::audio::streams
{
    class SoundStream final : public IAudioStream
    {
    public:
        explicit SoundStream(const std::shared_ptr<Sound>& sound);
        ~SoundStream();

        int readBuffer(int16_t* buffer, const size_t numSamples) override;
        bool isStereo() const override;
        int getRate() const override;
        bool endOfData() const override;

        scummvm::SoundHandle* getSoundHandlePtr() noexcept;
        std::weak_ptr<Sound>  getSound() const noexcept;
    private:
        std::shared_ptr<Sound> _sound;
        audio::scummvm::SoundHandle _handle;
        int _curPos = 0;
        int _bitsFactor;
    };
}
