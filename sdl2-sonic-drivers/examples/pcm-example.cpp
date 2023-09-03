#include <audio/scummvm/Mixer.hpp>
#include <audio/scummvm/SDLMixerManager.hpp>
#include <drivers/PCMDriver.hpp>
#include <files/WAVFile.hpp>
#include <files/VOCFile.hpp>
#include <audio/Sound.hpp>

#include <iostream>
#include <SDL2/SDL.h>

using namespace HyperSonicDrivers;

using namespace audio::scummvm;
using drivers::PCMDriver;
using std::cout;
using std::endl;

int main(int argc, char* argv[])
{
    SdlMixerManager mixerManager;
    mixerManager.init();

    auto mixer = mixerManager.getMixer();
    auto wavFile = std::make_shared<files::WAVFile>("Wav_868kb.wav");
    auto vocFile = std::make_shared<files::VOCFile>("DUNE.VOC");
    auto wavSound = wavFile->getSound();
    auto vocSound = vocFile->getSound();

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

    drv.play(wavSound, 150, -127);
    drv.play(vocSound, 255, 127);
    for (int i = 0, sig = +1; i < 3; i++, sig *= -1)
    {
        cout << i << ". playing same sound again reversed balance" << endl;
        SDL_Delay(200);
        drv.play(wavSound, 150, 127 * sig);
        drv.play(vocSound, 255, -127 * sig);
    }

    while(drv.isPlaying())
    {
        cout << "is playing" << endl;
        SDL_Delay(1000);
    }

    return 0;
}
