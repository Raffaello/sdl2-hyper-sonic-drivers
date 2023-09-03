#include <string>
#include <memory>
#include <format>
#include <cassert>
#include <HyperSonicDrivers/audio/scummvm/SDLMixerManager.hpp>

#include <SDL2/SDL_log.h>
#include <SDL2/SDL.h>

namespace HyperSonicDrivers::audio::scummvm
{
#if defined(GP2X)
#define SAMPLES_PER_SEC 11025
#elif defined(PLAYSTATION3) || defined(PSP2) || defined(NINTENDO_SWITCH)
#define SAMPLES_PER_SEC 48000
#else
#define SAMPLES_PER_SEC 44100
#endif

    SdlMixerManager::~SdlMixerManager()
    {
        SDL_CloseAudio();

        _mixer->setReady(false);

        SDL_QuitSubSystem(SDL_INIT_AUDIO);
    }

    uint8_t SDL_getBitsDepth(const SDL_AudioSpec& as)
    {
        uint8_t bits = as.format & 0xFF;
        SDL_LogDebug(SDL_LOG_CATEGORY_AUDIO, std::format("Audio {} bits", bits).c_str());

        return bits;
    }

    void SdlMixerManager::init()
    {
        // Start SDL Audio subsystem
        if (SDL_InitSubSystem(SDL_INIT_AUDIO) == -1) {
            SDL_LogError(SDL_LOG_CATEGORY_AUDIO, std::format("Could not initialize SDL: {}", std::string(SDL_GetError())).c_str());
            SDL_ClearError();
        }

#if SDL_VERSION_ATLEAST(2, 0, 0)
        const char* sdlDriverName = SDL_GetCurrentAudioDriver();
#else
        const int maxNameLen = 20;
        char sdlDriverName[maxNameLen];
        sdlDriverName[0] = '\0';
        SDL_AudioDriverName(sdlDriverName, maxNameLen);
#endif
        SDL_LogDebug(SDL_LOG_CATEGORY_AUDIO, std::format("Using SDL Audio Driver '{}'", sdlDriverName).c_str());
        // Get the desired audio specs
        SDL_AudioSpec desired = getAudioSpec(SAMPLES_PER_SEC);

        // Needed as SDL_OpenAudio as of SDL-1.2.14 mutates fields in
        // "desired" if used directly.
        SDL_AudioSpec fmt = desired;
        uint8_t bitsDepth = -1;
        // Start SDL audio with the desired specs
        if (SDL_OpenAudio(&fmt, &_obtained) != 0) {
            SDL_LogWarn(SDL_LOG_CATEGORY_AUDIO, std::format("Could not open audio device: {}", std::string(SDL_GetError())).c_str());
            SDL_ClearError();

            bitsDepth = SDL_getBitsDepth(fmt);
            // The mixer is not marked as ready
            //_mixer = new MixerImpl(desired.freq);
            _mixer = std::make_shared<MixerImpl>(desired.freq, bitsDepth);
            return;
        }

        bitsDepth = SDL_getBitsDepth(_obtained);
        // The obtained sample format is not supported by the mixer, call
        // SDL_OpenAudio again with NULL as the second argument to force
        // SDL to do resampling to the desired audio spec.
        if (_obtained.format != desired.format)
        {
            SDL_LogDebug(SDL_LOG_CATEGORY_AUDIO, std::format("SDL mixer sound format: {:#04x} differs from desired: {:#04x}", _obtained.format, desired.format).c_str());
            SDL_CloseAudio();

            if (SDL_OpenAudio(&fmt, NULL) != 0) {
                SDL_LogWarn(SDL_LOG_CATEGORY_AUDIO, std::format("Could not open audio device: {}", SDL_GetError()).c_str());
                // The mixer is not marked as ready
                //_mixer = new MixerImpl(desired.freq);
                _mixer = std::make_shared<MixerImpl>(desired.freq, bitsDepth);
                return;
            }

            _obtained = desired;
            bitsDepth = SDL_getBitsDepth(_obtained);
        }

        SDL_LogDebug(SDL_LOG_CATEGORY_AUDIO, std::format("Output sample rate: {} Hz", _obtained.freq).c_str());
        if (_obtained.freq != desired.freq) {
            SDL_LogWarn(SDL_LOG_CATEGORY_AUDIO, std::format("SDL mixer output sample rate: {} differs from desired: {}", _obtained.freq, desired.freq).c_str());
        }

        SDL_LogDebug(SDL_LOG_CATEGORY_AUDIO, std::format("Output buffer size: {}samples", _obtained.samples).c_str());
        if (_obtained.samples != desired.samples) {
            SDL_LogWarn(SDL_LOG_CATEGORY_AUDIO, std::format("SDL mixer output buffer size: {} differs from desired: {}", _obtained.samples, desired.samples).c_str());
        }

        SDL_LogDebug(SDL_LOG_CATEGORY_AUDIO, std::format("Output format: {}", _obtained.format).c_str());
        if (_obtained.format != desired.format) {
            SDL_LogWarn(SDL_LOG_CATEGORY_AUDIO, std::format("SDL mixer format: {} differs from desired: {}", _obtained.format, desired.format).c_str());
        }

#ifndef __SYMBIAN32__
        // The SymbianSdlMixerManager does stereo->mono downmixing,
        // but otherwise we require stereo output.
        if (_obtained.channels != 2) {
            SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "SDL mixer output requires stereo output device");
        }
#endif

        //_mixer = new MixerImpl(_obtained.freq);
        _mixer = std::make_shared<MixerImpl>(_obtained.freq, bitsDepth);
        assert(_mixer);
        _mixer->setReady(true);

        startAudio();
    }

    static uint32_t roundDownPowerOfTwo(uint32_t samples) {
        // Public domain code from Sean Eron Anderson
        // http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
        uint32_t rounded = samples;
        --rounded;
        rounded |= rounded >> 1;
        rounded |= rounded >> 2;
        rounded |= rounded >> 4;
        rounded |= rounded >> 8;
        rounded |= rounded >> 16;
        ++rounded;

        if (rounded != samples)
            rounded >>= 1;

        return rounded;
    }

    SDL_AudioSpec SdlMixerManager::getAudioSpec(uint32_t outputRate) {
        SDL_AudioSpec desired;

        // There was once a GUI option for this, which was removed. Configurability
        // is retained for advanced users only who wish to use the commandline
        // option (--output-rate) or modify their ScummVM config file directly.
        uint32_t freq = 0;
        //if (ConfMan.hasKey("output_rate"))
        //    freq = ConfMan.getInt("output_rate");
        if (freq <= 0)
            freq = outputRate;

        // One SDL "sample" is a complete audio frame (i.e. all channels = 1 sample)
        uint32_t samples = 0;

        // Different games and host systems have different performance
        // characteristics which are not easily measured, so allow advanced users to
        // tweak their audio buffer size if they are experience excess latency or
        // drop-outs by setting this value in their ScummVM config file directly
        //if (ConfMan.hasKey("audio_buffer_size", Common::ConfigManager::kApplicationDomain))
        //    samples = ConfMan.getInt("audio_buffer_size", Common::ConfigManager::kApplicationDomain);

        // 256 is an arbitrary minimum; 32768 is the largest power-of-two value
        // representable with uint16
        if (samples < 256 || samples > 32768)
            // By default, hold no more than 45ms worth of samples to avoid
            // perceptable audio lag (ATSC IS-191). For reference, DOSBox (as of Sep
            // 2017) uses a buffer size of 1024 samples by default for a 16-bit
            // stereo 44kHz mixer, which happens to be the next lowest power of two
            // below 45ms.
            samples = freq / (1000.0 / 45);

        memset(&desired, 0, sizeof(desired));
        desired.freq = freq;
        desired.format = AUDIO_S16SYS;
        desired.channels = 2;
        desired.samples = roundDownPowerOfTwo(samples);
        desired.callback = sdlCallback;
        desired.userdata = this;

        return desired;
    }

    void SdlMixerManager::startAudio() {
        // Start the sound system
        SDL_PauseAudio(0);
    }

    void SdlMixerManager::callbackHandler(uint8_t* samples, int len) {
        assert(_mixer);
        _mixer->mixCallback(samples, len);
    }

    void SdlMixerManager::sdlCallback(void* this_, uint8_t* samples, int len) {
        SdlMixerManager* manager = (SdlMixerManager*)this_;
        assert(manager);

        manager->callbackHandler(samples, len);
    }

    void SdlMixerManager::suspendAudio() {
        SDL_CloseAudio();
        _audioSuspended = true;
    }

    int SdlMixerManager::resumeAudio() {
        if (!_audioSuspended)
            return -2;
        if (SDL_OpenAudio(&_obtained, NULL) < 0) {
            return -1;
        }
        SDL_PauseAudio(0);
        _audioSuspended = false;
        return 0;
    }
}
