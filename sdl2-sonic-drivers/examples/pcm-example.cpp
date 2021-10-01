#include <audio/scummvm/Mixer.hpp>
#include <audio/scummvm/SDLMixerManager.hpp>
#include <drivers/PCMDriver.hpp>
#include <files/WAVFile.hpp>
#include <files/VOCFile.hpp>
#include <audio/Sound.hpp>

#include <iostream>
#include <SDL2/SDL.h>

using namespace audio::scummvm;
using drivers::PCMDriver;
using std::cout;
using std::endl;

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

    while (!mixer->isReady()) {
        cout << "mixer not ready" << endl;
        SDL_Delay(100);
    }

    drv.play(wavSound);
    while(drv.isPlaying(wavSound))
    {
        cout << "is playing" << endl;
        SDL_Delay(1000);
    }

    SDL_Delay(500);

    drv.play(vocSound);
    while (drv.isPlaying(vocSound))
    {
        cout << "is playing" << endl;
        SDL_Delay(1000);
    }

    SDL_Delay(500);

    drv.play(wavSound);
    drv.play(vocSound);
    while(drv.isPlaying(vocSound) || drv.isPlaying(wavSound))
    {
        cout << "is playing" << endl;
        SDL_Delay(1000);
    }

    return 0;
}
