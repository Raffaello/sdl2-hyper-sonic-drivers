#include <audio/scummvm/Mixer.hpp>
#include <audio/scummvm/SDLMixerManager.hpp>
#include <drivers/VOCDriver.hpp>
#include <files/VOCFile.hpp>
#include <spdlog/spdlog.h>
#include <SDL2/SDL.h>

int main(int argc, char* argv[])
{
    using namespace audio::scummvm;
    using  drivers::VOCDriver;

    SdlMixerManager mixerManager;
    mixerManager.init();

    std::shared_ptr<Mixer> mixer = mixerManager.getMixer();

    std::shared_ptr<files::VOCFile> vocFile = std::make_shared<files::VOCFile>("DUNE.VOC");

    VOCDriver voc(mixer, vocFile);
    voc.play();

    while (!mixer->isReady()) {
        spdlog::info("mixer not ready");
        SDL_Delay(100);
    }
   
    while (voc.isPlaying())
    {
        spdlog::info("is playing");
        SDL_Delay(1000);

    }

    spdlog::info("quitting...");
    SDL_Delay(1000);

    return 0;
}
