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
        //test only mono
        int len = numSamples / 2;
        int rest = (_dataSize - _curPos);
        int remaining = std::min(len, rest);
        
        for (int i = 0; i < remaining; i += 2) {
            float f = ((float)_data[_curPos++] - 128.0) / 255.0;
            buffer[i] = buffer[i + 1] = (int16_t)(f * 32767);
        }

        return remaining;

        /*
        // TODO convert PCM 8 bit to 16 bit
        int monoFactor = _stereo ? 1 : 2;
        int len = numSamples / monoFactor; // mixer is stereo
        int rest = (_dataSize - _curPos);
        int remaining = std::min(len, rest);
        
        // if voc is stereo.
        if (_stereo) {
            // TODO test
            for (int i = 0; i < remaining; i++) {
                buffer[i] = _data[_curPos + i];
            }

            _curPos += remaining;
        }
        else {
            // mono VO
            for (int i = 0; i < remaining * 2; i += 2) {
                float f = ((float)_data[_curPos++] - 128.0) / 255.0;
                buffer[i] = buffer[i + 1] = (int16_t)(f*32767) ;
            }
        }


        if (rest < len) {
           // std::memset(&buffer[_curPos], 0, (len - rest)*monoFactor);
        }

        return remaining;
        */
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
