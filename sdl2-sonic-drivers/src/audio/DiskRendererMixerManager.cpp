#include <audio/DiskRendererMixerManager.hpp>
#include <audio/scummvm/MixerImpl.hpp>
#include <memory>
#include <stdexcept>
#include <spdlog/spdlog.h>

namespace audio
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
    void DiskRendererMixerManager::startRecording(std::string filename)
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
            spdlog::warn("File not open");
        }
    }
    void DiskRendererMixerManager::rendererCallback(void* this_, uint8_t* samples, int len)
    {
        DiskRendererMixerManager* manager = reinterpret_cast<DiskRendererMixerManager*>(this_);

        manager->callbackHandler(samples, len);
    }
}