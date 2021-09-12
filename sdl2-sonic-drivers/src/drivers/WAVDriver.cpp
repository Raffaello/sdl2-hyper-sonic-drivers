#include <drivers/WAVDriver.hpp>
#include <utils/endianness.hpp>

namespace drivers
{
    using audio::scummvm::Mixer;
    using audio::scummvm::SoundHandle;
    using audio::scummvm::AudioStream;
    using utils::READ_LE_UINT16;

    WAVDriver::WAVDriver(std::shared_ptr<audio::scummvm::Mixer> mixer, std::shared_ptr<files::WAVFile> wav_file) :
        _mixer(mixer), _wav_file(wav_file)
    {
        _handle = new SoundHandle();
        files::WAVFile::format_t format = _wav_file->getFormat();
        _stereo = format.channels == 2;
        _sampleRate = format.samplesPerSec;
        _dataSize = _wav_file->getDataSize();
        _data = _wav_file->getData();
        _bitsDepth = format.bitsPerSample;
        _bitsFactor = _bitsDepth == 16 ? 2 : 1;
    }

    WAVDriver::~WAVDriver()
    {
        delete _handle;
    }

    int WAVDriver::readBuffer(int16_t* buffer, const int numSamples)
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
    bool WAVDriver::isStereo() const
    {
        return _stereo;
    }
    int WAVDriver::getRate() const
    {
        return _sampleRate;
    }
    bool WAVDriver::endOfData() const
    {
        return _curPos == _dataSize;;
    }
    bool WAVDriver::isPlaying() const noexcept
    {
        return _mixer->isSoundHandleActive(*_handle);
    }
    void WAVDriver::play()
    {
        // TODO review, could be speech instead of SFX or other
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
}
