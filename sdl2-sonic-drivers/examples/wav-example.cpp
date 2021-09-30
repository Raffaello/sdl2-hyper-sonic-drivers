#include <audio/scummvm/Mixer.hpp>
#include <audio/scummvm/SDLMixerManager.hpp>
#include <drivers/WAVDriver.hpp>
#include <files/WAVFile.hpp>
#include <spdlog/spdlog.h>
#include <SDL2/SDL.h>

int main(int argc, char* argv[])
{
    using namespace audio::scummvm;
    using  drivers::WAVDriver;

    SdlMixerManager mixerManager;
    mixerManager.init();

    std::shared_ptr<Mixer> mixer = mixerManager.getMixer();

    std::shared_ptr<files::WAVFile> vocFile = std::make_shared<files::WAVFile>("Wav_868kb.wav");

    WAVDriver wav(mixer, vocFile);
    wav.play();

    while (!mixer->isReady()) {
        spdlog::info("mixer not ready");
        SDL_Delay(100);
    }

    while (wav.isPlaying())
    {
        spdlog::info("is playing");
        SDL_Delay(1000);

    }

    spdlog::info("quitting...");
    SDL_Delay(1000);

    return 0;
}
