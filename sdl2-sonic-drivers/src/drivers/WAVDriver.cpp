#include <drivers/WAVDriver.hpp>
#include <utils/endianness.hpp>

namespace drivers
{
    using audio::scummvm::Mixer;
    //using audio::scummvm::SoundHandle;
    //using audio::scummvm::AudioStream;
    //using utils::READ_LE_UINT16;

    WAVDriver::WAVDriver(std::shared_ptr<audio::scummvm::Mixer> mixer) :
        _mixer(mixer)
    {
    }

    WAVDriver::~WAVDriver()
    {
    }
    
    bool WAVDriver::isPlaying(const std::shared_ptr<audio::Sound> sound) const noexcept
    {
        return _mixer->isSoundHandleActive(*sound->getHandle());
    }

    void WAVDriver::play(const std::shared_ptr<audio::Sound> sound, const uint8_t volume)
    {
        // TODO review it,
        // BODY those constants and could be done in Sound class instead?
        // BODY this driver at the moment became just an helper class. not really usefull..
        _mixer->playStream(
            sound->getSoundType(),
            sound->getHandle(),
            sound.get(),
            -1,
            volume,
            0,
            false
        );
    }
}
