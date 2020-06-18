// sdl2-sonic-drivers.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <adl/sound_adlib.h>
#include <SDL_mixer.h>
#include <audio/softsynths/PCSpeaker.hpp>

using namespace std;

int adl()
{
    Mix_Init(0);
    if (Mix_OpenAudio(44100, AUDIO_S16, 2, 1024) < 0) {
        cerr << Mix_GetError();
        return -1;
    }

    //MIX_CHANNELS(8);
    //Mix_AllocateChannels(16);

    int freq;
    uint16_t fmt;
    int channels;
    if (Mix_QuerySpec(&freq, &fmt, &channels) == 0) {
        cerr << "query return 0" << endl;
    }
    cout << "freq: " << freq << endl
        << "format: " << fmt << endl
        << "channels: " << channels << endl;
    if (channels > 2) {
        // with 8 audio channels doesn't reproduce the right sound.
        // i guess is something that can be fixed
        // but i do not know why.
        // the code should be similar to scummVM or DosBox
        // so if it is working there, should work here.
        // it means this code is not really the same
        // need to start organizing in it properly.
        cerr << "CHANNELS not mono or stereo!" << endl;
    }

    SDL_RWops* adlFile = SDL_RWFromFile("DUNE0.ADL", "rb");
    if (nullptr == adlFile) {
        cerr << "file not found" << endl;
        throw std::runtime_error("file not found");
    }

    SoundAdlibPC adlib = SoundAdlibPC(adlFile);


    /*SDL_AudioSpec spec;
    SDL_AudioSpec obtained;
    spec.freq = 44100;
    spec.format = AUDIO_S16SYS;
    spec.channels = 2;
    spec.callback = adlib.callback;
    spec.userdata = &adlib;
    spec.samples = 1024;

    if (SDL_OpenAudio(&spec, &obtained) != 0) {
        cerr << "unable to open audio" << endl;
    }

    cout << "obtained AudioSpec: " << endl
        << "freq     = " << (int) obtained.freq << endl
        << "format   = " << (int) obtained.format << endl
        << "channels = " << (int) obtained.channels << endl
        << "samples  = " << (int) obtained.samples << endl;*/

    auto songs = adlib.getSubsongs();
    //adlib.setVolume(0xFF);

    //Mix_Volume(-1, MIX_MAX_VOLUME);

    cout << "Volume: " << adlib.getVolume() << endl;
    cout << "num Tracks: " << songs.size() << endl;

    adlib.playTrack(2);
    Mix_HookMusic(adlib.callback, &adlib);
    do {
        cout << "playin music, waiting 1s..." << endl;
        SDL_Delay(1000);
    } while (adlib.isPlaying());
    cout << "end";
    //for (auto& s : songs) {
    //	Mix_Chunk* chunk = adlib.getSubsong(s);
    //	cout << "song: " << s << endl;
    //	
    //	adlib.playTrack(s);
    //	Mix_HookMusic(adlib.callback, &adlib);
    //	cout << "channels playing: " << Mix_Playing(-1) << endl;

    //	/*while (Mix_Playing(-1)) {
    //		cout << "waiting 1s ..." << endl;
    //		SDL_Delay(1000);
    //	}*/
    //	SDL_Delay(1000);
    //	Mix_FreeChunk(chunk);
    //}

    //for (auto& s : songs) {
    //	Mix_Chunk* chunk = adlib.getSubsong(s);
    //	cout << "song: " << s << endl;
    //	int channel = Mix_PlayChannel(-1, chunk, 0);
    //	//Mix_HookMusic(adlib.callback, &adlib);
    //	cout << "channels playing: " << Mix_Playing(-1) << endl;
    //	
    //	while (Mix_Playing(-1)) { 
    //		cout << "waiting 1s ..." << endl;
    //		SDL_Delay(1000); 
    //	}
    //	SDL_Delay(1000);
    //	Mix_FreeChunk(chunk);
    //}

    SDL_RWclose(adlFile);
    Mix_HaltChannel(-1);
    Mix_HaltMusic();
    Mix_CloseAudio();
    Mix_Quit();

    return 0;
}

int pcspkr()
{
    using audio::softsynths::PCSpeaker;

    Mix_Init(0);
    if (Mix_OpenAudio(44100, AUDIO_S16, 2, 1024) < 0) {
        cerr << Mix_GetError();
        return -1;
    }

    int length = 3000;

    PCSpeaker pcSpeaker(44100, 8);
    pcSpeaker.volume = 8;
    cout << "isPlaying: " << pcSpeaker.isPlaying() << endl
        << "Rate: " << pcSpeaker.getRate() << endl
        << "vol: " << (int)pcSpeaker.volume << endl;
    

    // TODO try with channels.
    Mix_HookMusic(pcSpeaker.callback, &pcSpeaker);
    
    cout << "Pulse" << endl;
    pcSpeaker.play(PCSpeaker::eWaveForm::PULSE, 440, 300);
    while (pcSpeaker.isPlaying()) { SDL_Delay(100); }
    pcSpeaker.play(PCSpeaker::eWaveForm::PULSE, 440+183, 600);
    while (pcSpeaker.isPlaying()) { SDL_Delay(100); }
    SDL_Delay(600);

    cout << "SQUARE" << endl;
    pcSpeaker.play(PCSpeaker::eWaveForm::SQUARE, 440, 300);
    while (pcSpeaker.isPlaying()) { SDL_Delay(10); }
    pcSpeaker.play(PCSpeaker::eWaveForm::SQUARE, 440 + 183, 600);
    while (pcSpeaker.isPlaying()) { SDL_Delay(10); }
    SDL_Delay(600);
    
    cout << "SINE" << endl;
    pcSpeaker.play(PCSpeaker::eWaveForm::SINE, 440, 300);
    while (pcSpeaker.isPlaying()) { SDL_Delay(10); }
    pcSpeaker.play(PCSpeaker::eWaveForm::SINE, 440 + 183, 600);
    while (pcSpeaker.isPlaying()) { SDL_Delay(10); }
    SDL_Delay(600);
    
    cout << "SAW" << endl;
    pcSpeaker.play(PCSpeaker::eWaveForm::SAW, 440, 300);
    while (pcSpeaker.isPlaying()) { SDL_Delay(10); }
    pcSpeaker.play(PCSpeaker::eWaveForm::SAW, 440 + 183, 600);
    while (pcSpeaker.isPlaying()) { SDL_Delay(10); }
    SDL_Delay(600);
    
    cout << "TRINAGLE" << endl;
    pcSpeaker.play(PCSpeaker::eWaveForm::TRIANGLE, 440, 300);
    while (pcSpeaker.isPlaying()) { SDL_Delay(10); }
    pcSpeaker.play(PCSpeaker::eWaveForm::TRIANGLE, 440 + 183, 600);
    while (pcSpeaker.isPlaying()) { SDL_Delay(10); }
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

int main(int argc, char* argv[])
{
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
    
//    adl();
    pcspkr();


    //SDL_CloseAudio();
    SDL_Quit();
    return 0;
}
