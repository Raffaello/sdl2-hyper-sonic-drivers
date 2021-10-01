#include <audio/scummvm/Mixer.hpp>
#include <audio/scummvm/SDLMixerManager.hpp>
#include <drivers/PCMDriver.hpp>
#include <files/WAVFile.hpp>
#include <files/VOCFile.hpp>
#include <audio/Sound.hpp>

#include <spdlog/spdlog.h>
#include <SDL2/SDL.h>

using namespace audio::scummvm;
using  drivers::PCMDriver;


int voc_exmaple()
{
    SdlMixerManager mixerManager;
    mixerManager.init();

    std::shared_ptr<Mixer> mixer = mixerManager.getMixer();

    std::shared_ptr<files::VOCFile> vocFile = std::make_shared<files::VOCFile>("DUNE.VOC");

    PCMDriver voc(mixer);
    voc.play(vocFile->getSound());

    while (!mixer->isReady()) {
        spdlog::info("mixer not ready");
        SDL_Delay(100);
    }

    while (voc.isPlaying(vocFile->getSound()))
    {
        spdlog::info("is playing");
        SDL_Delay(1000);

    }

    spdlog::info("quitting...");
    SDL_Delay(1000);

    return 0;
}

int wav_example()
{
    SdlMixerManager mixerManager;
    mixerManager.init();

    std::shared_ptr<Mixer> mixer = mixerManager.getMixer();

    std::shared_ptr<files::WAVFile> wavFile = std::make_shared<files::WAVFile>("Wav_868kb.wav");
    std::shared_ptr<audio::Sound> wavSound = wavFile->getSound();

    PCMDriver wav(mixer);

    wav.play(wavSound);

    while (!mixer->isReady()) {
        spdlog::info("mixer not ready");
        SDL_Delay(100);
    }

    while (wav.isPlaying(wavSound))
    {
        spdlog::info("is playing");
        SDL_Delay(1000);
    }

    spdlog::info("quitting...");
    SDL_Delay(1000);

    return 0;
}

int main(int argc, char* argv[])
{
    SdlMixerManager mixerManager;
    mixerManager.init();

    std::shared_ptr<Mixer> mixer = mixerManager.getMixer();

    std::shared_ptr<files::WAVFile> wavFile = std::make_shared<files::WAVFile>("Wav_868kb.wav");
    std::shared_ptr<files::VOCFile> vocFile = std::make_shared<files::VOCFile>("DUNE.VOC");

    std::shared_ptr<audio::Sound> wavSound = wavFile->getSound();
    std::shared_ptr<audio::Sound> vocSound = vocFile->getSound();

    PCMDriver drv(mixer);

    drv.play(wavSound);

    while (!mixer->isReady()) {
        spdlog::info("mixer not ready");
        SDL_Delay(100);
    }

    while(drv.isPlaying(wavSound))
    {
        spdlog::info("is playing");
        SDL_Delay(1000);
    }

    SDL_Delay(500);

    drv.play(vocSound);
    while (drv.isPlaying(vocSound))
    {
        spdlog::info("is playing");
        SDL_Delay(1000);

    }

    SDL_Delay(500);

    drv.play(wavSound);
    drv.play(vocSound);
    while(drv.isPlaying(vocSound) || drv.isPlaying(wavSound))
    {
        spdlog::info("is playing");
        SDL_Delay(1000);
    }

    return 0;


    return 0;




}
