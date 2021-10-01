#pragma once

#include <audio/scummvm/Mixer.hpp>
#include <files/WAVFile.hpp>
#include <audio/Sound.hpp>
#include <memory>

namespace drivers
{
    // TODO generalize in PCMDriver class both for WAV and VOC
    // BODY: this class is basicaly a copy of VOCDriver
    class PCMDriver
    {
    public:
        PCMDriver(std::shared_ptr<audio::scummvm::Mixer> mixer);
        virtual ~PCMDriver();
        
        //void setFile() noexcept;

        /*virtual int readBuffer(int16_t* buffer, const int numSamples);
        virtual bool isStereo() const;
        virtual int getRate() const;
        virtual bool endOfData() const;*/

        bool isPlaying(const std::shared_ptr<audio::Sound> sound) const noexcept;
        void play(const std::shared_ptr<audio::Sound> sound, const uint8_t volume = audio::scummvm::Mixer::MaxVolume::CHANNEL);

    private:
        std::shared_ptr<audio::scummvm::Mixer> _mixer;
        //std::shared_ptr<files::WAVFile> _wav_file;

        //bool _stereo;
        //int _sampleRate;
        //int _dataSize;
        //int _curPos;
        //uint8_t _bitsDepth;
        //int _bitsFactor;
        //std::shared_ptr<uint8_t[]> _data;
        //audio::scummvm::SoundHandle* _handle;
    };
}
