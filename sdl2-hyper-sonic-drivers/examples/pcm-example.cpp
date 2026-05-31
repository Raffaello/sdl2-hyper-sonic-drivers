#include <HyperSonicDrivers/audio/mixer/ChannelGroup.hpp>
#include <HyperSonicDrivers/drivers/PCMDriver.hpp>
#include <HyperSonicDrivers/files/WAVFile.hpp>
#include <HyperSonicDrivers/files/VOCFile.hpp>
#include <HyperSonicDrivers/audio/PCMSound.hpp>
#include <HyperSonicDrivers/utils/algorithms.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>
#include <HyperSonicDrivers/files/loaders.hpp>

#if HAS_SDL3
#include <HyperSonicDrivers/audio/sdl3/Mixer.hpp>
#include <SDL3/SDL_main.h>
#define MIXER audio::sdl3::Mixer
#elif HAS_SDL2
#include <HyperSonicDrivers/audio/sdl2/Mixer.hpp>
#include <SDL2/SDL_main.h>
#define MIXER audio::sdl2::Mixer
#else
#include <HyperSonicDrivers/audio/rtaudio/Mixer.hpp>
#define MIXER audio::rtaudio::Mixer
#endif

#include <iostream>


using namespace HyperSonicDrivers;

using audio::mixer::eChannelGroup;
using drivers::PCMDriver;
using std::cout;
using std::endl;
using utils::logI;
using utils::delayMillis;

int main(int argc, char* argv[])
{
    auto mixer = audio::make_mixer<MIXER>(8, 44100, 1024);
    if (!mixer->init())
    {
        utils::logI("can't init mixer");
        return 1;
    }

    auto wavSound = files::loadSoundFromFile<files::WAVFile>("Wav_868kb.wav", eChannelGroup::Speech);
    auto vocSound = files::loadSoundFromFile<files::VOCFile>("DUNE.VOC", eChannelGroup::Speech);

    PCMDriver drv(mixer);

    while (!mixer->isReady())
    {
        cout << "mixer not ready" << endl;
        delayMillis(100);
    }

    drv.play(wavSound);
    while (drv.isPlaying(wavSound))
    {
        cout << "is playing" << endl;
        delayMillis(1000);
    }

    delayMillis(500);

    drv.play(vocSound);
    while (drv.isPlaying(vocSound))
    {
        cout << "is playing" << endl;
        delayMillis(1000);
    }

    delayMillis(500);

    drv.play(wavSound, 150, 127);
    drv.play(vocSound, 255, -127);
    for (int i = 0, sig = +1; i < 3; i++, sig *= -1)
    {
        cout << i << ". playing same sound again reversed balance" << endl;
        delayMillis(500);
        drv.play(wavSound, 150, 127 * sig);
        drv.play(vocSound, 255, -127 * sig);
    }

    while (drv.isPlaying())
    {
        cout << "is playing" << endl;
        delayMillis(1000);
    }

    return 0;
}
