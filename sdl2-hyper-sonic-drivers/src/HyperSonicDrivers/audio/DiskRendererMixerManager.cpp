#include <memory>
#include <stdexcept>
#include <HyperSonicDrivers/audio/DiskRendererMixerManager.hpp>
#include <HyperSonicDrivers/audio/scummvm/MixerImpl.hpp>
#include <SDL2/SDL_log.h>

namespace HyperSonicDrivers::audio
{
    DiskRendererMixerManager::DiskRendererMixerManager(const int rate, const uint8_t bits, const uint8_t channels) :
        _rate(rate), _bits(bits), _channels(channels)
    {
    }

    DiskRendererMixerManager::DiskRendererMixerManager(const int rate) : DiskRendererMixerManager(rate, 16, 2)
    {
    }

    DiskRendererMixerManager::~DiskRendererMixerManager()
    {
        _mixer->setReady(false);
        if (_file.is_open())
        {
            _file.close();
        }
    }
    void DiskRendererMixerManager::init()
    {
        _mixer = std::make_shared<scummvm::MixerImpl>(_rate, _bits);
        _mixer->setReady(true);
        
    }
    void DiskRendererMixerManager::suspendAudio()
    {
        //_mixer->pauseAll();
    }
    int DiskRendererMixerManager::resumeAudio()
    {
        return 0;
    }
    void DiskRendererMixerManager::startRecording(const std::string& filename)
    {
        _file.open(filename, std::fstream::out | std::fstream::binary);
        if (!_file) {
            throw std::runtime_error("unable to create file");
        }
    }

    void DiskRendererMixerManager::callbackHandler(uint8_t* samples, int len)
    {
        // this forces to have stereo 16 bits...
        //_mixer->mixCallback(samples, len);
        //write samples to disk
        if (_file.is_open()) {
            _file.write(reinterpret_cast<const char*>(samples), len);
        }
        else {
            SDL_LogWarn(SDL_LOG_CATEGORY_AUDIO, "File not open");
        }
    }
    void DiskRendererMixerManager::rendererCallback(void* this_, uint8_t* samples, int len)
    {
        DiskRendererMixerManager* manager = reinterpret_cast<DiskRendererMixerManager*>(this_);

        manager->callbackHandler(samples, len);
    }
}