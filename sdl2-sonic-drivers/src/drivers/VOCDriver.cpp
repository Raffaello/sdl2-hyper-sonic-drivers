#include <drivers/VOCDriver.hpp>

namespace drivers
{
    using audio::scummvm::Mixer;
    using audio::scummvm::SoundHandle;
    using audio::scummvm::AudioStream;

    VOCDriver::VOCDriver(std::shared_ptr<audio::scummvm::Mixer> mixer, std::shared_ptr<files::VOCFile> voc_file) :
        _mixer(mixer), _voc_file(voc_file)
    {
        _handle = new SoundHandle();

        // convert VOC File to PCM data
       // _voc_file->_data_blocks
    }

    VOCDriver::~VOCDriver()
    {
        delete _handle;
    }

    int VOCDriver::readBuffer(int16_t* buffer, const int numSamples)
    {
        
        return -1;
    }

    bool VOCDriver::isStereo() const
    {
        return false;
    }

    int VOCDriver::getRate() const
    {
        return 0;
    }

    bool VOCDriver::endOfData() const
    {
        return false;
    }


    void VOCDriver::play()
    {
        // TODO review, could be speech instead of SFX
        _block_index = 0;
        _mixer->playStream(
            Mixer::SoundType::SFX,
            _handle,
            this
        );
    }
}
