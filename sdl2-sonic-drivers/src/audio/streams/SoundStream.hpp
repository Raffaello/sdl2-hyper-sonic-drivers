#pragma once

#include <audio/Sound.hpp>
#include <audio/scummvm/AudioStream.hpp>
#include <audio/scummvm/SoundHandle.hpp>
#include <cstdint>
#include <memory>

namespace audio
{
    namespace streams
    {
        class SoundStream final : public scummvm::AudioStream
        {
        public:
            //SoundStream(const SoundStream&) = delete;
            SoundStream(const std::shared_ptr<Sound> sound);
            ~SoundStream();

            virtual int readBuffer(int16_t* buffer, const int numSamples) override;
            virtual bool isStereo() const override;
            virtual int getRate() const override;
            virtual bool endOfData() const override;
            
            scummvm::SoundHandle* getSoundHandlePtr() noexcept;
            std::weak_ptr<Sound>  getSound() const noexcept;
        private:
            std::shared_ptr<Sound> _sound;
            audio::scummvm::SoundHandle _handle;
            int _curPos = 0;
            int _bitsFactor;
        };
    }
}