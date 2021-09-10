#pragma once

#include <audio/scummvm/Mixer.hpp>
#include <audio/scummvm/SoundHandle.hpp>
#include <audio/scummvm/AudioStream.hpp>
#include <files/VOCFile.hpp>
#include <memory>
#include <vector>

namespace drivers
{
    class VOCDriver : protected audio::scummvm::AudioStream
    {
    public:
        VOCDriver(std::shared_ptr<audio::scummvm::Mixer> mixer, std::shared_ptr<files::VOCFile> voc_file);
        virtual ~VOCDriver();

        //void setFile() noexcept;

        virtual int readBuffer(int16_t* buffer, const int numSamples);
        virtual bool isStereo() const;
        virtual int getRate() const;
        virtual bool endOfData() const;


        void play();
        void play(float speed);
        void play(int rate);

    private:
        std::shared_ptr<audio::scummvm::Mixer> _mixer;
        std::shared_ptr<files::VOCFile> _voc_file;

        bool _stereo;
        int _sampleRate;
        int _dataSize;
        int _curPos;
        std::shared_ptr<uint8_t[]> _data;

        //std::vector<std::unique_ptr<audio::scummvm::SoundHandle>> _handles;
        audio::scummvm::SoundHandle* _handle;
        //audio::scummvm::AudioStream* _stream;
        //int _block_index;
    };
}
