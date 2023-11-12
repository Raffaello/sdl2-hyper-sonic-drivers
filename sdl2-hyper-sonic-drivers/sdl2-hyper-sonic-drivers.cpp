﻿// sdl2-hyper-sonic-drivers.cpp : Defines the entry point for the application.
// TODO: delete this file and its target, this is kinda scratch pad

#include <iostream>
#include <memory>

#include <SDL2/SDL.h>

#include <HyperSonicDrivers/drivers/westwood/ADLDriver.hpp>
#include <HyperSonicDrivers/files/miles/XMIFile.hpp>
#include <HyperSonicDrivers/files/westwood/ADLFile.hpp>
#include <HyperSonicDrivers/hardware/PCSpeaker.hpp>
#include <HyperSonicDrivers/hardware/opl/OPLFactory.hpp>

#include <HyperSonicDrivers/hardware/opl/mame/MameOPL3.hpp>


#include <HyperSonicDrivers/files/MIDFile.hpp>

#include <HyperSonicDrivers/utils/algorithms.hpp>


#include <HyperSonicDrivers/drivers/MIDDriver.hpp>

#include <HyperSonicDrivers/files/dmx/MUSFile.hpp>
#include <HyperSonicDrivers/files/dmx/OP2File.hpp>


#include <HyperSonicDrivers/audio/sdl2/Mixer.hpp>
#include <HyperSonicDrivers/utils/sdl2/Logger.hpp>
#include <HyperSonicDrivers/devices/Adlib.hpp>
#include <HyperSonicDrivers/audio/sdl2/Renderer.hpp>
#include <mt32emu/c_interface/cpp_interface.h>
#include <HyperSonicDrivers/hardware/mt32/MT32.hpp>

#include <HyperSonicDrivers/utils/sound.hpp>
#include <HyperSonicDrivers/drivers/PCMDriver.hpp>
#include <HyperSonicDrivers/audio/PCMSound.hpp>


using namespace std;
using namespace HyperSonicDrivers;

void newMixerTest()
{
    using namespace audio;

    utils::sdl2::Logger::instance->setLevelAll(utils::ILogger::eLevel::Trace);

    //auto mixer = sdl2::Mixer(8, 44100, 1024);
    auto mixer = make_mixer<sdl2::Mixer>(8, 44100, 1024);
    if (!mixer->init())
        return;

    if (!mixer->isReady())
        return;

    utils::logI("OK");
}

void playNotes(hardware::PCSpeaker *pcSpeaker, const hardware::PCSpeaker::eWaveForm waveForm, const int freq, const int length)
{
    auto start = std::chrono::steady_clock::now();
    pcSpeaker->play(waveForm, freq, length);
    while (pcSpeaker->isPlaying()) { SDL_Delay(10); }
    pcSpeaker->play(waveForm, freq + 183, length * 2);
    while (pcSpeaker->isPlaying()) { SDL_Delay(10); }
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "Elapsed Time (s) = " << elapsed_seconds.count() << " --- Expected (s) ~=" << length + length * 2 << "\n";
}

/*
int pcspkr(const int freq, const uint16_t audio, const int channels,const int chunksize)
{
    using hardware::PCSpeaker;
 
    Mix_Init(0);
    if (Mix_OpenAudio(freq, audio, channels, chunksize) < 0) {
        cerr << Mix_GetError();
        return -1;
    }

    int length = 3000;
    int freq_;
    uint16_t fmt;
    int chn;
    int8_t bits = 0;
    bool sig = false;
    
    Mix_QuerySpec(&freq_, &fmt, &chn);
    switch (fmt)
    {
    case AUDIO_S32:
        bits = 32;
        sig = true;
        break;
    case AUDIO_F32:
        bits = 32;
        sig = false; //encoded unsiged 32 bit as float.
        break;
    case AUDIO_S16:
        bits = 16;
        sig = true;
        break;
    case AUDIO_U16:
        bits = 16;
        sig = false;
        break;
    case AUDIO_S8:
        bits = 8;
        sig = true;
        break;
    case AUDIO_U8:
        bits = 8;
        sig = false;
        break;
    default:
        std::cerr << "not valid parameter" << endl;
        return -1;
    }

    PCSpeaker pcSpeaker(freq_, chn, bits, sig);
    //pcSpeaker.volume = 8;
    cout << "isPlaying: " << pcSpeaker.isPlaying() << endl
        << "Rate (Hz) : " << (int)pcSpeaker.getRate() << endl
        << "Channels  : " << (int)pcSpeaker.getChannels() << endl
        << "Bits      : " << (int)pcSpeaker.getBits() << endl
        << "Signed    : " << std::boolalpha << pcSpeaker.getSigned() << std::noboolalpha << endl;
    

    // TODO try with channels.
    Mix_HookMusic(pcSpeaker.callback, &pcSpeaker);
    
    cout << "SQUARE" << endl;
    playNotes(&pcSpeaker, PCSpeaker::eWaveForm::SQUARE, 440, 300);
    SDL_Delay(600);
    
    cout << "SINE" << endl;
    playNotes(&pcSpeaker, PCSpeaker::eWaveForm::SINE, 440, 300);
    SDL_Delay(600);
    
    cout << "SAW" << endl;
    playNotes(&pcSpeaker, PCSpeaker::eWaveForm::SAW, 440, 300);
    SDL_Delay(600);
    
    cout << "TRINAGLE" << endl;
    playNotes(&pcSpeaker, PCSpeaker::eWaveForm::TRIANGLE, 440, 300);
    SDL_Delay(600);
    cout << "isPlaying: " << pcSpeaker.isPlaying() << endl;;
    SDL_Delay(100);
    cout << "isPlaying: " << pcSpeaker.isPlaying();

    Mix_HaltChannel(-1);
    Mix_HaltMusic();
    Mix_CloseAudio();
    Mix_Quit();
    return 0;
}
*/
/*
int teen()
{
    using hardware::PCSpeaker;

    Mix_Init(0);
    if (Mix_OpenAudio(44100, AUDIO_S16, 2, 1024) < 0) {
        cerr << Mix_GetError();
        return -1;
    }

    PCSpeaker pcSpeaker(44100, 8);
    cout << "isPlaying: " << pcSpeaker.isPlaying() << endl
        << "Rate: " << pcSpeaker.getRate() << endl;
    
    const int notes[] = {
        //       A4  A4#   B4  C5   C5#  D5
                440, 466, 494, 523, 554, 587,
                //  D5#  E5   F5   F#5  G5   G#5  A5
                    622, 659, 698, 740, 784, 830, 880 };


    const int notes2[] = {
//       A    A#   B    C    C#   D    D#   E    F    F#   G    G#   
        220, 233, 247, 262, 277, 294, 311, 330, 349, 370, 392, 415,  // 3 - 4
        440, 466, 494, 523, 554, 587, 622, 659, 698, 740, 784, 830,  // 4 - 5
        880                                                          // 5
    };
    const int major[] = { 0, 2, 4, 5, 7, 9, 11, 12 };
    // C5, D#5, F5, G#4, D#5-F5-D#5, C#5-C5, C5#-C5, A#5 G5#, C5-C#5-C5, G#5-G5, G#5-G5, G#5-G5, G#5-G5-G5-F5
    // 1   1    2   4    1  0.5 0.5, 1   2   1   2    1  2
    const int song2[]    = { 3+12, 6+12, 8+12, 11, 6+12,8+12,6+12, 4+12,3+12, 4+12,3+12, 1+12, 11, 1+12,3+12,1+12, 11,10, 11,10, 11,10, 11,10,10,8,  11,10, 11,10, 11,10, 11,10,10,8 };
    const int songInt2[] = { 2,    4,    6,    8,  4,   4,   6,    4,   6,    4,   6,    6,    6,  4,   4,   4,    4, 6,  4, 6,  4, 6,  4, 4, 4, 8,  4, 6,  4, 6,  4, 6,  4, 4, 4, 16 };
    
    // TODO try with channels.
    Mix_HookMusic(pcSpeaker.callback, &pcSpeaker);

    cout << "Pulse" << endl;
   /* for (int n = 0; n < 8; n++) {
        cout << "note: " << notes[major[n]] << endl;
        pcSpeaker.play(PCSpeaker::eWaveForm::PULSE, notes[major[n]], 500);
        while (pcSpeaker.isPlaying()) { SDL_Delay(10); }
    }* /
    
    SDL_Delay(500);
    cout << "isPlaying: " << pcSpeaker.isPlaying();
    for (int n = 0; n < 36; n++) {
        int length = songInt2[n] * 25;
        pcSpeaker.play(PCSpeaker::eWaveForm::SQUARE, notes2[song2[n]], length);
        while (pcSpeaker.isPlaying()) { SDL_Delay(10); }
    }

    Mix_HaltChannel(-1);
    Mix_HaltMusic();
    Mix_CloseAudio();
    Mix_Quit();
    return 0;
}
*/

/*
int song()
{
    using hardware::PCSpeaker;

    Mix_Init(0);
    if (Mix_OpenAudio(44100, AUDIO_S16, 2, 1024) < 0) {
        cerr << Mix_GetError();
        return -1;
    }

    int length = 500;

    PCSpeaker pcSpeaker(44100, 8);
    cout << "isPlaying: " << pcSpeaker.isPlaying() << endl
        << "Rate: " << pcSpeaker.getRate() << endl;

    const int notes[] = {
        //       A4  A4#   B4  C5   C5#  D5
                440, 466, 494, 523, 554, 587,
                //  D5#  E5   F5   F#5  G5   G#5  A5
                    622, 659, 698, 740, 784, 830, 880 };


    const int notes2[] = {
        //       A    A#   B    C    C#   D    D#   E    F    F#   G    G#   
                220, 233, 247, 262, 277, 294, 311, 330, 349, 370, 392, 415,  // 3 - 4
                440, 466, 494, 523, 554, 587, 622, 659, 698, 740, 784, 830,  // 4 - 5
                880                                                          // 5
    };
    const int major[] = { 0, 2, 4, 5, 7, 9, 11, 12 };
    // C5, D#5, F5, G#4, D#5-F5-D#5, C#5-C5, C5#-C5, A#5 G5#, C5-C#5-C5, G#5-G5, G#5-G5, G#5-G5, G#5-G5-G5-F5
    // 1   1    2   4    1  0.5 0.5, 1   2   1   2    1  2
    const int song2[] = { 3 + 12, 6 + 12, 8 + 12, 11, 6 + 12,8 + 12,6 + 12, 4 + 12,3 + 12, 4 + 12,3 + 12, 1 + 12, 11, 1 + 12,3 + 12,1 + 12, 11,10, 11,10, 11,10, 11,10,10,8,  11,10, 11,10, 11,10, 11,10,10,8 };
    const int songInt2[] = { 2,    4,    6,    8,  4,   4,   6,    4,   6,    4,   6,    6,    6,  4,   4,   4,    4, 6,  4, 6,  4, 6,  4, 4, 4, 8,  4, 6,  4, 6,  4, 6,  4, 4, 4, 16 };

    // TODO try with channels.
    Mix_HookMusic(pcSpeaker.callback, &pcSpeaker);

    cout << "Pulse" << endl;
    for (int n = 0; n < 8; n++) {
         cout << "note: " << notes[major[n]] << endl;
         pcSpeaker.play(PCSpeaker::eWaveForm::SQUARE, notes[major[n]], 250);
         while (pcSpeaker.isPlaying()) { SDL_Delay(10); }
     }

    Mix_HaltChannel(-1);
    Mix_HaltMusic();
    Mix_CloseAudio();
    Mix_Quit();
    return 0;
}
*/

//int renderMixer()
//{
//    using namespace audio::scummvm;
//    using namespace hardware::opl::scummvm;
//    using namespace hardware::opl;
//    using namespace drivers::westwood;
//
//    audio::DiskRendererMixerManager mixerManager(44100);
//    mixerManager.init();
//    mixerManager.startRecording("test.dat");
//
//    std::shared_ptr<Mixer> mixer = mixerManager.getMixer();
//
//    //spdlog::set_level(spdlog::level::debug);
//    auto opl = OPLFactory::create(OplEmulator::NUKED, OplType::OPL3, mixer);
//    auto pOpl = dynamic_cast<EmulatedOPL*>( opl.get());
//    //auto opl = std::make_shared<hardware::opl::mame::MameOPL>(mixer);
//    std::shared_ptr<files::westwood::ADLFile> adlFile = std::make_shared<files::westwood::ADLFile>("test/fixtures/DUNE0.ADL");
//
//    ADLDriver adlDrv(opl, adlFile);
//    adlDrv.play(4, 0xFF);
//    int samples = -1;
//    int totSamples = 0;
//    bool isPlaying = adlDrv.isPlaying();
//    do
//    {
//        // TODO review, but is dumping the data
//        int16_t buf[1024];
//
//        samples = pOpl->readBuffer(buf, 1024);
//        mixerManager.callbackHandler(reinterpret_cast<uint8_t*>(buf), samples * 2);
//        totSamples += samples;
//        isPlaying = adlDrv.isPlaying();
//        //spdlog::info("isPlaying? {}", isPlaying);
//    } while (isPlaying);
//
//    //spdlog::info("TotSamples={} --- space require={} ({}KB) [{}MB]", totSamples, totSamples * sizeof(int16_t), totSamples * sizeof(int16_t) / 1024, totSamples * sizeof(int16_t) / 1024 / 1024);
//
//    while (!mixer->isReady()) {
//        //spdlog::info("mixer not ready");
//        utils::delayMillis(100);
//    }
//
//    utils::delayMillis(1000);
//    while (adlDrv.isPlaying())
//    {
//        //spdlog::info("is playing");
//        utils::delayMillis(100);
//
//    }
//
//    //spdlog::info("renderer quitting...");
//
//    return 0;
//}

void rendererMIDI()
{
    // TODO: need to review the MIDDrv as it is time dependant
    //       and it is not good for rendering midi.
    //       at the moment the simpler thing would be 
    //       creating a MIDI_Renderer driver
    //       just to achieve the result, that will skip the
    //       delays.
    //       or better change the driver instead of using a detached thread
    //       that is keeping track of the time.
    //       set up the callback for the emulated OPL
    //       that is triggered by the device, to process the next MIDI events.
    //       or something.
    using hardware::opl::OplEmulator;
    using hardware::opl::OplType;
    using audio::mixer::eChannelGroup;
    using utils::ILogger;
    using hardware::opl::OPL;

    //audio::sdl2::Renderer r(44100, 1024);

    //r.setOutputFile("renderer_midi.wav");

    //auto mixer = r.getMixer();
    //auto op2f = files::dmx::OP2File("test/fixtures/GENMIDI.OP2");
    //auto midi_adlib = std::make_shared<devices::Adlib>(mixer, audio::mixer::eChannelGroup::Music);
    //auto mid_drv = drivers::MIDDriver(midi_adlib);
    //auto mus = files::dmx::MUSFile("test/fixtures/D_E1M1.mus");
    //mid_drv.play(mus.getMIDI());
    //auto eo = midi_adlib->getOpl();

    //// TODO: doesn't work, due to the driver internal timing
    //while (mid_drv.isPlaying())
    //    r.renderBuffer(eo);

    //r.releaseOutputFile();

    //files::WAVFile w("renderer.wav");
    //auto sound = w.getSound();
}

void testMT32()
{
    utils::ILogger::instance->setLevelAll(utils::ILogger::eLevel::Info);

    const std::string cr = "mt32_roms/MT32_CONTROL.1987-10-07.v1.07.ROM";
    const std::string pr = "mt32_roms/MT32_PCM.ROM";

    auto mixer = audio::make_mixer<audio::sdl2::Mixer>(8, 44100, 1024);
    if (!mixer->init())
        std::cerr << "can't init mixer" << std::endl;
    //auto midi_mt32 = devices::make_midi_device<devices::midi::MidiMT32>(mixer, cr, pr);

    std::shared_ptr<files::MIDFile> midFile = std::make_shared<files::MIDFile>("test/fixtures/MI_intro.mid");
    //drivers::MIDDriver midDrv(midi_mt32);

    //midDrv.play(midFile->getMIDI());
    //while (midDrv.isPlaying())
    //    utils::delayMillis(1000);
}


void pcm_sound_append()
{
    auto mixer = audio::make_mixer<audio::sdl2::Mixer>(8, 44100, 1024);
    files::WAVFile w1("test/fixtures/test_renderer_adlib_mame2.wav");
    
    mixer->init();
    auto s1 = w1.getSound();
    auto s1b = w1.getSound();
    //auto s2a = utils::makeMono(s1);
    //auto s2b = utils::makeStereo(s1);
    auto s2 = utils::append(s1, s1);
    drivers::PCMDriver drv(mixer);

    drv.play(s2);
    while (drv.isPlaying())
    {
        utils::delayMillis(100);
    }
}

int main(int argc, char* argv[])
{
    //newMixerTest();
    //testMultiOpl();
    //testMOplMultiDrv();
    //rendererMIDI();
    //midi_adlib();
    //testMT32();

    pcm_sound_append();
    return 0;
    //sdlMixer();
    //SDL_Delay(100);
    //renderMixer();

    //xmi_parser();
    //midi_adlib_mus_file_CONCURRENCY_ERROR_ON_SAME_DEVICE();
    //midi_adlib_mus_op2_file();
    //midi_adlib_xmi();

    SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO);

    int numAudioDevices = SDL_GetNumAudioDevices(0);
    cout << "numAudioDevices = " << numAudioDevices << endl;
    //int numAudioDevices2 = SDL_GetNumAudioDevices(1);
    //cout << "numAudioDevices2 = " << numAudioDevices2 << endl;

    for (int i = 0; i < numAudioDevices; i++) {
        cout << "Device " << i << " name: " << SDL_GetAudioDeviceName(i, 0) << endl;
    }

    int numAudioDrivers = SDL_GetNumAudioDrivers();
    cout << "numAudioDrivers = " << numAudioDrivers << endl;
    for (int i = 0; i < numAudioDrivers; i++) {
        cout << "Driver " << i << " name: " << SDL_GetAudioDriver(i) << endl;
    }
    
    SDL_AudioSpec spec = {};
    spec.callback = NULL;
    spec.channels = 1;
    spec.format = AUDIO_U8;
    spec.freq = 22050;
    spec.samples = 512;
    spec.silence = 0;

    SDL_OpenAudio(&spec, &spec);

    cout << "channels " << (int) spec.channels << endl
        << "freq " << spec.freq << endl
        << "format " << (int)spec.format << endl;

    SDL_CloseAudio();

    //drivers::miles::XMidi::readDriver("ALGDIG.ADV");
    //files::XMIFile f("SPKRDEMO.XMI");
    //files::ADLFile f("EOBSOUND.ADL");
    //cout << "ADL VERSION: " << f.getVersion() << endl;
        
    
    // TODO: 32 bit audio
    //pcspkr(44100, AUDIO_S32, 2, 1024);
    //pcspkr(44100, AUDIO_F32, 2, 1024);
    //pcspkr(44100, AUDIO_S16, 2, 1024);
    //pcspkr(44100, AUDIO_S8, 2, 1024);
    //pcspkr(44100, AUDIO_U16, 2, 1024);
    //pcspkr(44100, AUDIO_U8, 2, 1024);

    //teen();
    //song();


    SDL_Quit();
    return 0;
}
