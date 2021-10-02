#include <audio/Sound.hpp>
#include <utils/endianness.hpp>


namespace audio
{
    using utils::READ_LE_UINT16;
    using audio::scummvm::SoundHandle;

    Sound::Sound(const scummvm::Mixer::SoundType soundType, const bool isStereo, const int rate, const uint8_t bitsDepth, const uint32_t dataSize, const std::shared_ptr<uint8_t[]> data)
        : _soundType(soundType), _stereo(isStereo), _rate(rate), _bitsDepth(bitsDepth), _dataSize(dataSize), _data(data), _curPos(0)
    {
        _bitsFactor = _bitsDepth == 16 ? 2 : 1;
    }

    int Sound::readBuffer(int16_t* buffer, const int numSamples)
    {
        int len = numSamples;
        int rest = (_dataSize - _curPos) / _bitsFactor;
        int remaining = std::min(len, rest);

        for (int i = 0; i < remaining; i++) {
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
    
    bool Sound::isStereo() const
    {
        return _stereo;
    }
    
    int Sound::getRate() const
    {
        return _rate;
    }
    
    bool Sound::endOfData() const
    {
        return _curPos == _dataSize;
    }

    uint8_t Sound::getBitsDepth() const noexcept
    {
        return _bitsDepth;
    }

    const scummvm::Mixer::SoundType Sound::getSoundType() const noexcept
    {
        return _soundType;
    }
}
