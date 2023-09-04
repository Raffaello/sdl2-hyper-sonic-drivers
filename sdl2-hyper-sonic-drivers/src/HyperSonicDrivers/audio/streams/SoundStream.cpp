#include <HyperSonicDrivers/audio/streams/SoundStream.hpp>
#include <HyperSonicDrivers/utils/endianness.hpp>
#include <cassert>

namespace HyperSonicDrivers::audio::streams
{
    using utils::READ_LE_UINT16;
    using audio::scummvm::SoundHandle;

    SoundStream::SoundStream(const std::shared_ptr<Sound>& sound)
        : _sound(sound)
    {
        _handle = SoundHandle();
        _bitsFactor = _sound->bitsDepth == 16 ? 2 : 1;
    }

    SoundStream::~SoundStream()
    {
    }

    int SoundStream::readBuffer(int16_t* buffer, const int numSamples)
    {
        assert(_sound->dataSize % _bitsFactor == 0);

        int len = numSamples;
        int rest = (_sound->dataSize - _curPos) / _bitsFactor;
        int remaining = std::min(len, rest);

        for (int i = 0; i < remaining; i++) {
            // TODO convert Audio stream before playback?
            if (_sound->bitsDepth == 8) {
                buffer[i] = static_cast<int16_t>((_sound->data[_curPos++] - 128) * 128);
            }
            else {
                buffer[i] = READ_LE_UINT16(&_sound->data[_curPos]);
                _curPos += 2;
            }
        }

        assert(_curPos <= _sound->dataSize);
        return remaining;
    }

    bool SoundStream::isStereo() const
    {
        return _sound->stereo;
    }

    int SoundStream::getRate() const
    {
        return _sound->rate;
    }

    bool SoundStream::endOfData() const
    {
        return _curPos == _sound->dataSize;
    }

    scummvm::SoundHandle* SoundStream::getSoundHandlePtr() noexcept
    {
        return &_handle;
    }

    std::weak_ptr<Sound> SoundStream::getSound() const noexcept
    {
        return _sound;
    }
}
