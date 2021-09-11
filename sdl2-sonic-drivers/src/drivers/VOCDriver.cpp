#include <drivers/VOCDriver.hpp>
#include <cassert>

namespace drivers
{
    using audio::scummvm::Mixer;
    using audio::scummvm::SoundHandle;
    using audio::scummvm::AudioStream;

    VOCDriver::VOCDriver(std::shared_ptr<audio::scummvm::Mixer> mixer, std::shared_ptr<files::VOCFile> voc_file) :
        _mixer(mixer), _voc_file(voc_file)
    {
        _handle = new SoundHandle();
        _stereo = voc_file->getChannels() == 2;
        _sampleRate = voc_file->getSampleRate();
        _dataSize = voc_file->getDataSize();
        _data = voc_file->getData();

        // convert VOC File to PCM data
       // _voc_file->_data_blocks
    }

    VOCDriver::~VOCDriver()
    {
        delete _handle;
    }

    int VOCDriver::readBuffer(int16_t* buffer, const int numSamples)
    {
        // TODO review, only mone played twice the sample rate is ok.
        // it should be a clash with mono stereo?
        //test only mono
        int len = numSamples;
        int rest = (_dataSize - _curPos);
        int remaining = std::min(len, rest);
        
        for (int i = 0; i < remaining; i ++) {
            // TODO generalize and do it better.
            // 8 bit  unsigend to 16 bit signed conversion
            //float f = ((float)_data[_curPos++] - 128.0) / 256.0;
            //buffer[i] = (int16_t)(f * 32767);
            buffer[i] = (int16_t)((_data[_curPos++] - 128) * 128);
        }

        return remaining;
    }

    bool VOCDriver::isStereo() const
    {
        return _stereo;
    }

    int VOCDriver::getRate() const
    {
        return _sampleRate;
    }

    bool VOCDriver::endOfData() const
    {
        return _curPos == _dataSize;
    }


    void VOCDriver::play()
    {
        // TODO review, could be speech instead of SFX
        _curPos = 0;
        _mixer->playStream(
            Mixer::SoundType::SFX,
            _handle,
            this,
            -1,
            Mixer::MaxVolume::CHANNEL,
            0,
            false
        );
    }

    void VOCDriver::play(float speed)
    {
        // TODO
        _sampleRate *= speed;
        play();
    }

    void VOCDriver::play(int rate)
    {
        // TODO
        _sampleRate = rate;
        play();
    }
}
