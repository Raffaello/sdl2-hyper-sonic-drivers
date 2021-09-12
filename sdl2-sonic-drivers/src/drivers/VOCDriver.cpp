#include <drivers/VOCDriver.hpp>
#include <cassert>
#include <utils/endianness.hpp>

namespace drivers
{
    using audio::scummvm::Mixer;
    using audio::scummvm::SoundHandle;
    using audio::scummvm::AudioStream;
    using utils::READ_LE_UINT16;

    VOCDriver::VOCDriver(std::shared_ptr<audio::scummvm::Mixer> mixer, std::shared_ptr<files::VOCFile> voc_file) :
        _mixer(mixer), _voc_file(voc_file)
    {
        _handle = new SoundHandle();
        _stereo = voc_file->getChannels() == 2;
        _sampleRate = voc_file->getSampleRate();
        _dataSize = voc_file->getDataSize();
        _data = voc_file->getData();
    }

    VOCDriver::~VOCDriver()
    {
        delete _handle;
    }

    int VOCDriver::readBuffer(int16_t* buffer, const int numSamples)
    {
        int len = numSamples;
        int rest = (_dataSize - _curPos) / _bitsFactor;
        int remaining = std::min(len, rest);
        
        for (int i = 0; i < remaining; i ++) {
            // TODO convert Audio stream before playback?
            if (_bitsDepth == 8) {
                buffer[i] = (int16_t)((_data[_curPos++] - 128) * 128);
            }
            else {
                buffer[i] = READ_LE_UINT16(&_data[_curPos]);
                _curPos += 2;
            }
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

    bool VOCDriver::isPlaying() const noexcept
    {
        return _mixer->isSoundHandleActive(*_handle);
    }


    void VOCDriver::play()
    {
        // TODO review, could be speech instead of SFX or other
        _curPos = 0;
        _bitsDepth = _voc_file->getBitsDepth();
        _bitsFactor = _bitsDepth == 16 ? 2 : 1;
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
}
