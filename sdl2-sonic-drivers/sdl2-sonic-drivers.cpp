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
    int freq;
    uint16_t fmt;
    int chn;
    
    Mix_QuerySpec(&freq, &fmt, &chn);

    PCSpeaker pcSpeaker(freq, chn);
    pcSpeaker.volume = 8;
    cout << "isPlaying: " << pcSpeaker.isPlaying() << endl
        << "Rate: " << pcSpeaker.getRate() << endl
        << "vol: " << (int)pcSpeaker.volume << endl;
    

    // TODO try with channels.
    Mix_HookMusic(pcSpeaker.callback, &pcSpeaker);
    
    cout << "Pulse" << endl;
    pcSpeaker.play(PCSpeaker::eWaveForm::PULSE, 440, 300);
    while (pcSpeaker.isPlaying()) { SDL_Delay(10); }
    pcSpeaker.play(PCSpeaker::eWaveForm::PULSE, 440+183, 600);
    while (pcSpeaker.isPlaying()) { SDL_Delay(10); }
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

int teen()
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
    }*/
    
    SDL_Delay(500);
    cout << "isPlaying: " << pcSpeaker.isPlaying();
    for (int n = 0; n < 36; n++) {
        int length = songInt2[n] * 100;
        pcSpeaker.play(PCSpeaker::eWaveForm::PULSE, notes2[song2[n]], length);
        while (pcSpeaker.isPlaying()) { SDL_Delay(10); }
    }

    Mix_HaltChannel(-1);
    Mix_HaltMusic();
    Mix_CloseAudio();
    Mix_Quit();
    return 0;
}

int song()
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
         pcSpeaker.play(PCSpeaker::eWaveForm::PULSE, notes[major[n]], 500);
         while (pcSpeaker.isPlaying()) { SDL_Delay(10); }
     }

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
    //teen();
    //song();


    //SDL_CloseAudio();
    SDL_Quit();
    return 0;
}
