#include <drivers/PCMDriver.hpp>

namespace drivers
{
    using audio::scummvm::Mixer;

    PCMDriver::PCMDriver(std::shared_ptr<audio::scummvm::Mixer> mixer) :
        _mixer(mixer)
    {
    }
   
    bool PCMDriver::isPlaying(const std::shared_ptr<audio::Sound> sound) const noexcept
    {
        return _mixer->isSoundHandleActive(*sound->getHandle());
    }

    void PCMDriver::play(const std::shared_ptr<audio::Sound> sound, const uint8_t volume)
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
