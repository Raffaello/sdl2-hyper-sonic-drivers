#pragma once

#include <audio/scummvm/Mixer.hpp>
#include <audio/scummvm/SoundHandle.hpp>
#include <audio/scummvm/AudioStream.hpp>
#include <files/WAVFile.hpp>
#include <memory>

namespace drivers
{
    // TODO generalize in PCMDriver class both for WAV and VOC
    // BODY: this class is basicaly a copy of VOCDriver
    class WAVDriver : protected audio::scummvm::AudioStream
    {
    public:
        WAVDriver(std::shared_ptr<audio::scummvm::Mixer> mixer, std::shared_ptr<files::WAVFile> wav_file);
        virtual ~WAVDriver();
        
        //void setFile() noexcept;

        virtual int readBuffer(int16_t* buffer, const int numSamples);
        virtual bool isStereo() const;
        virtual int getRate() const;
        virtual bool endOfData() const;

        bool isPlaying() const noexcept;
        void play();

    private:
        std::shared_ptr<audio::scummvm::Mixer> _mixer;
        std::shared_ptr<files::WAVFile> _wav_file;

        bool _stereo;
        int _sampleRate;
        int _dataSize;
        int _curPos;
        uint8_t _bitsDepth;
        int _bitsFactor;
        std::shared_ptr<uint8_t[]> _data;
        audio::scummvm::SoundHandle* _handle;
    };
}
