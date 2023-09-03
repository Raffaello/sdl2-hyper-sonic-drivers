#pragma once

#include <cstdint>
#include <string>
#include <fstream> // TODO replace with WAVFile
#include <HyperSonicDrivers/audio/scummvm/MixerManager.hpp>

namespace HyperSonicDrivers::audio
{
    // TODO requires to have a timer callback to set up
    // BIDY at the actual status is just experimental.
    class DiskRendererMixerManager : public scummvm::MixerManager
    {
    public:
        DiskRendererMixerManager(const int rate, const uint8_t bits, const uint8_t channels);
        DiskRendererMixerManager(const int rate);
        virtual ~DiskRendererMixerManager();

        virtual void init();
        virtual void suspendAudio();
        virtual int resumeAudio();
        void startRecording(std::string filename);
    //protected:
        //virtual void startAudio();
        virtual void callbackHandler(uint8_t* samples, int len);
        static void rendererCallback(void* this_, uint8_t* samples, int len);

    private:
        int _rate;
        uint8_t _bits;
        uint8_t _channels;
        std::fstream _file;
    };
    
}
