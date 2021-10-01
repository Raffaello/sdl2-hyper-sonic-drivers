#include <audio/scummvm/Mixer.hpp>
#include <audio/scummvm/SDLMixerManager.hpp>
#include <drivers/WAVDriver.hpp>
#include <files/WAVFile.hpp>
#include <audio/Sound.hpp>
#include <drivers/WAVDriver.hpp>

#include <spdlog/spdlog.h>
#include <SDL2/SDL.h>

int main(int argc, char* argv[])
{
    using namespace audio::scummvm;
    using  drivers::WAVDriver;

    SdlMixerManager mixerManager;
    mixerManager.init();

    std::shared_ptr<Mixer> mixer = mixerManager.getMixer();

    std::shared_ptr<files::WAVFile> wavFile = std::make_shared<files::WAVFile>("Wav_868kb.wav");
    std::shared_ptr<audio::Sound> wavSound = wavFile->getSound();
    
    WAVDriver wav(mixer);
    
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
