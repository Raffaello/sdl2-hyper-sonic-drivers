// sdl2-sonic-drivers.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <adl/sound_adlib.h>
#include <SDL2/SDL.h>
#include <SDL_mixer.h>

#include <hardware/PCSpeaker.hpp>
#include <drivers/miles/XMidi.hpp>
#include <files/XMIFile.hpp>
#include <files/ADLFile.hpp>
#include <drivers/westwood/woody/ADLDriver.hpp>
#include <hardware/opl/woody/SurroundOPL.hpp>

#include <drivers/westwood/ADLDriver.hpp>
#include <audio/SDL2Mixer.hpp>
#include <hardware/opl/scummvm/mame/mame.hpp>
#include <spdlog/spdlog.h>

#include <hardware/opl/scummvm/dosbox/dosbox.hpp>
#include <hardware/opl/scummvm/Config.hpp>

#include <hardware/opl/scummvm/nuked/OPL.hpp>

using namespace std;

int adl()
{
    Mix_Init(0);
    if (Mix_OpenAudio(44100, AUDIO_S16, 2, 1024) < 0) {
        cerr << Mix_GetError();
        return -1;
    }


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

    auto songs = adlib.getSubsongs();

    cout << "Volume: " << adlib.getVolume() << endl;
    cout << "num Tracks: " << songs.size() << endl;

    adlib.playTrack(4);
    Mix_HookMusic(adlib.callback, &adlib);
    do {
        cout << "playin music, waiting 1s..." << endl;
        SDL_Delay(1000);
    } while (adlib.isPlaying());
    cout << "end";
 
    SDL_RWclose(adlFile);
    Mix_HaltChannel(-1);
    Mix_HaltMusic();
    Mix_CloseAudio();
    Mix_Quit();

    return 0;
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
    }*/
    
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

void ADLDRV_callback_woody(void* userdata, Uint8* audiobuf, int len)
{
    drivers::westwood::woody::ADLDriver* _driver = static_cast<drivers::westwood::woody::ADLDriver*>(userdata);
    //hardware::opl::woody::SurroundOPL* sOpl = reinterpret_cast<hardware::opl::woody::SurroundOPL*>(userdata);
    
    int16_t* buf = reinterpret_cast<int16_t*>(audiobuf);
    //sOpl->readBuffer(buf, len / 2 );
    
    int samples = _driver->readBuffer(buf, len / 2*2); //stereo 16 bit => *2 channels, /2 16 bits 

    //int volume = 128;
    //for (int i = 0; i < samples; i++) {
    //    //printf("0x%x\n", buf[i]);
    //    buf[i] = static_cast<int16_t>(buf[i] );
    //}

    //self->bJustStartedPlaying = false;
}

int adl_driver_woody()
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

    std::shared_ptr<audio::SDL2Mixer> mixer = std::make_shared<audio::SDL2Mixer>();
    std::shared_ptr<hardware::opl::woody::SurroundOPL> opl = std::make_shared<hardware::opl::woody::SurroundOPL>(mixer);

    std::shared_ptr<files::ADLFile> adlFile = std::make_shared<files::ADLFile>("DUNE0.ADL");
    drivers::westwood::woody::ADLDriver adlDrv(opl.get(), adlFile);
    adlDrv.initDriver();

    adlDrv.startSound(2, 128);
    Mix_HookMusic(ADLDRV_callback_woody, &adlDrv);
//    Mix_HookMusic(adlib.callback, &adlib);
   // do {
        //cout << "playin music, waiting 1s..." << endl;
        SDL_Delay(4000);
 //   } while (adlDrv.isChannelPlay);

    SDL_Delay(3000);
    Mix_HaltChannel(-1);
    Mix_HaltMusic();
    Mix_CloseAudio();
    Mix_Quit();

    return 0;

}

void callback_mame(void* userdata, uint8_t* stream, int len)
{
    // TODO
    // Sound is played from ADLDriver not from OPL ?
    // But is feeded to OPL ?
    // and genereated from OPL ?
    // I need the ADLDriver here ? ....

    // don't understand why the buffer is simply fill of zeros....
    //static bool first = true;
    //if (!first) return;
    //std::fstream wf("440Hz.dat", ios::out | ios::binary);
    //if (!wf) return;

    hardware::opl::scummvm::mame::OPL* opl = reinterpret_cast<hardware::opl::scummvm::mame::OPL*>(userdata);
    int16_t* buf = reinterpret_cast<int16_t*>(stream);
    // / 2 because of sterio and opl is mono .... just for testing.
    opl->readBuffer(buf, len / 2 / 2);
    
    
    //wf.write((char*)stream, len);
    //wf.close();
    //first = false;
}

void callback_dosbox(void* userdata, uint8_t* stream, int len)
{
    // TODO: merge into 1 callback
    

    // don't understand why the buffer is simply fill of zeros....
    //static bool first = true;
    //if (!first) return;
    //std::fstream wf("440Hz.dat", ios::out | ios::binary);
    //if (!wf) return;

    hardware::opl::scummvm::dosbox::OPL* opl = reinterpret_cast<hardware::opl::scummvm::dosbox::OPL*>(userdata);
    int16_t* buf = reinterpret_cast<int16_t*>(stream);
    // / 2 because of sterio and opl is mono .... just for testing.
    opl->readBuffer(buf, len / 2);


    //wf.write((char*)stream, len);
    //wf.close();
    //first = false;
}

void callback_nuked(void* userdata, uint8_t* stream, int len)
{
    // TODO: merge into 1 callback


    // don't understand why the buffer is simply fill of zeros....
    //static bool first = true;
    //if (!first) return;
    //std::fstream wf("440Hz.dat", ios::out | ios::binary);
    //if (!wf) return;

    hardware::opl::scummvm::nuked::OPL* opl = reinterpret_cast<hardware::opl::scummvm::nuked::OPL*>(userdata);
    int16_t* buf = reinterpret_cast<int16_t*>(stream);
    // / 2 because of sterio and opl is mono .... just for testing.
    opl->readBuffer(buf, len / 2);


    //wf.write((char*)stream, len);
    //wf.close();
    //first = false;
}

void callback_surround(void* userdata, uint8_t* stream, int len)
{
    // TODO: merge into 1 callback


    // don't understand why the buffer is simply fill of zeros....
    //static bool first = true;
    //if (!first) return;
    //std::fstream wf("440Hz.dat", ios::out | ios::binary);
    //if (!wf) return;

    hardware::opl::woody::SurroundOPL* opl = reinterpret_cast<hardware::opl::woody::SurroundOPL*>(userdata);
    int16_t* buf = reinterpret_cast<int16_t*>(stream);
    opl->readBuffer(buf, len / 2);
}

/* These are offsets from the base I/O address. */
constexpr int FM = 8;       // SB (mono) ports (e.g. 228H and 229H)
constexpr int PROFM1 = 0;   // On CT-1330, this is left OPL-2.  On CT-1600 and
                            // later cards, it's OPL-3 bank 0.
constexpr int PROFM2 = 2;   // On CT-1330, this is right OPL-2.  On CT-1600 and
                            // later cards, it's OPL-3 bank 1.

void FMoutput(unsigned port, int reg, int val, std::shared_ptr<hardware::opl::OPL> opl)
/* This outputs a value to a specified FM register at a specified FM port. */
{
    opl->write(port, reg);
    SDL_Delay(8);
    opl->write(port + 1, val);
    SDL_Delay(55);
}

void fm(int reg, int val, std::shared_ptr<hardware::opl::OPL> opl)
/* This function outputs a value to a specified FM register at the Sound
 * Blaster (mono) port address.
 */
{
    FMoutput(FM, reg, val, opl);
}

void Profm1(int reg, int val, std::shared_ptr<hardware::opl::OPL> opl)
/* This function outputs a value to a specified FM register at the Sound
 * Blaster Pro left FM port address (or OPL-3 bank 0).
 */
{
    FMoutput(PROFM1, reg, val, opl);
}

void Profm2(int reg, int val, std::shared_ptr<hardware::opl::OPL> opl)
/* This function outputs a value to a specified FM register at the Sound
 * Blaster Pro right FM port address (or OPL-3 bank 1).
 */
{
    FMoutput(PROFM2, reg, val, opl);
}

void opl2_test(std::shared_ptr<hardware::opl::OPL> opl)
{
    opl->init();
    opl->setCallbackFrequency(72);

    fm(1, 0, opl);        /* must initialize this to zero */
    fm(0xC0, 1, opl);     /* parallel connection */

   /***************************************
    * Set parameters for the carrier cell *
    ***************************************/

    fm(0x23, 0x21, opl);  /* no amplitude modulation (D7=0), no vibrato (D6=0),
                     * sustained envelope type (D5=1), KSR=0 (D4=0),
                     * frequency multiplier=1 (D4-D0=1)
                     */

    fm(0x43, 0x0, opl);   /* no volume decrease with pitch (D7-D6=0),
                     * no attenuation (D5-D0=0)
                     */

    fm(0x63, 0xff, opl);  /* fast attack (D7-D4=0xF) and decay (D3-D0=0xF) */
    fm(0x83, 0x05, opl);  /* high sustain level (D7-D4=0), slow release rate (D3-D0=5) */


    /*****************************************
     * Set parameters for the modulator cell *
     *****************************************/

    fm(0x20, 0x20, opl);  /* sustained envelope type, frequency multiplier=0    */
    fm(0x40, 0x3f, opl);  /* maximum attenuation, no volume decrease with pitch */

    /* Since the modulator signal is attenuated as much as possible, these
     * next two values shouldn't have any effect.
     */
    fm(0x60, 0x44, opl);  /* slow attack and decay */
    fm(0x80, 0x05, opl);  /* high sustain level, slow release rate */


    /*************************************************
     * Generate tone from values looked up in table. *
     *************************************************/

    printf("440 Hz tone, values looked up in table.\n");
    fm(0xa0, 0x41, opl);  /* 440 Hz */
    fm(0xb0, 0x32, opl);  /* 440 Hz, block 0, key on */

    SDL_Delay(1000);

    fm(0xb0, 0x12, opl);  /* key off */
}

void dual_opl2_test(std::shared_ptr<hardware::opl::OPL> opl)
{
    opl->init();
    opl->setCallbackFrequency(72);

    fm(1, 0, opl);        /* must initialize this to zero */
    fm(0xC0, 1, opl);     /* parallel connection */

    /***************************************
    * Set parameters for the carrier cell *
    ***************************************/

    fm(0x23, 0x21, opl);  /* no amplitude modulation (D7=0), no vibrato (D6=0),
                     * sustained envelope type (D5=1), KSR=0 (D4=0),
                     * frequency multiplier=1 (D4-D0=1)
                     */

    fm(0x43, 0x0, opl);   /* no volume decrease with pitch (D7-D6=0),
                     * no attenuation (D5-D0=0)
                     */

    fm(0x63, 0xff, opl);  /* fast attack (D7-D4=0xF) and decay (D3-D0=0xF) */
    fm(0x83, 0x05, opl);  /* high sustain level (D7-D4=0), slow release rate (D3-D0=5) */


    /*****************************************
     * Set parameters for the modulator cell *
     *****************************************/

    fm(0x20, 0x20, opl);  /* sustained envelope type, frequency multiplier=0    */
    fm(0x40, 0x3f, opl);  /* maximum attenuation, no volume decrease with pitch */

    /* Since the modulator signal is attenuated as much as possible, these
     * next two values shouldn't have any effect.
     */
    fm(0x60, 0x44, opl);  /* slow attack and decay */
    fm(0x80, 0x05, opl);  /* high sustain level, slow release rate */


    /*************************************************
     * Generate tone from values looked up in table. *
     *************************************************/

    spdlog::info("440 Hz tone, values looked up in table.\n");
    fm(0xa0, 0x41, opl);  /* 440 Hz */
    fm(0xb0, 0x32, opl);  /* 440 Hz, block 0, key on */

    SDL_Delay(1000);

    fm(0xb0, 0x12, opl);  /* key off */

    SDL_Delay(1000);
    // ---------------- left / right test

    // TODO: opl->isStereo();
    // TODO: opl->getType();

    // only dual opl
    spdlog::info("Left/Right DUAL OPL2");
    int block = 4;        /* choose block=4 and m=1 */
    int m = 1;		       /* m is the frequency multiple number */
    int f = 440;          /* want f=440 Hz */
    int b = 1 << block;
    /* This is the equation to calculate frequency number from frequency. */
    int fn = (long)f * 1048576 / b / m / 50000L;
    constexpr int KEYON = 0x20;     // key-on bit in regs b0 - b8

    fm(0xA0, (fn & 0xFF), opl);
    fm(0xB0, ((fn >> 8) & 0x3) + (block << 2) | KEYON, opl);
    SDL_Delay(1000);
    fm(0xB0, ((fn >> 8) & 0x3) + (block << 2), opl);       // key off
    SDL_Delay(1000);

    spdlog::info("Left channel only\n");

    Profm1(0xB0, ((fn >> 8) & 0x3) + (block << 2) | KEYON, opl);

    SDL_Delay(1000);
    Profm1(0xB0, ((fn >> 8) & 0x3) + (block << 2), opl);   // key off
    SDL_Delay(1000);
    spdlog::info("Right channel only\n");
    Profm2(0xB0, ((fn >> 8) & 0x3) + (block << 2) | KEYON, opl);
    SDL_Delay(1000);

    fm(0xb0, 0x12, opl);  /* key off */
}

void opl3_test(std::shared_ptr<hardware::opl::OPL> opl)
{
    constexpr auto LEFT = 0x10;
    constexpr auto RIGHT = 0x20;

    opl->init();
    opl->setCallbackFrequency(72);

    fm(1, 0, opl);        /* must initialize this to zero */
    Profm2(5, 1, opl);  /* set to OPL3 mode, necessary for stereo */
    fm(0xC0, LEFT | RIGHT | 1, opl);     /* set both channels, parallel connection */

    /***************************************
    * Set parameters for the carrier cell *
    ***************************************/

    fm(0x23, 0x21, opl);  /* no amplitude modulation (D7=0), no vibrato (D6=0),
                     * sustained envelope type (D5=1), KSR=0 (D4=0),
                     * frequency multiplier=1 (D4-D0=1)
                     */

    fm(0x43, 0x0, opl);   /* no volume decrease with pitch (D7-D6=0),
                     * no attenuation (D5-D0=0)
                     */

    fm(0x63, 0xff, opl);  /* fast attack (D7-D4=0xF) and decay (D3-D0=0xF) */
    fm(0x83, 0x05, opl);  /* high sustain level (D7-D4=0), slow release rate (D3-D0=5) */


    /*****************************************
     * Set parameters for the modulator cell *
     *****************************************/

    fm(0x20, 0x20, opl);  /* sustained envelope type, frequency multiplier=0    */
    fm(0x40, 0x3f, opl);  /* maximum attenuation, no volume decrease with pitch */

    /* Since the modulator signal is attenuated as much as possible, these
     * next two values shouldn't have any effect.
     */
    fm(0x60, 0x44, opl);  /* slow attack and decay */
    fm(0x80, 0x05, opl);  /* high sustain level, slow release rate */


    /*************************************************
     * Generate tone from values looked up in table. *
     *************************************************/

    spdlog::info("440 Hz tone, values looked up in table.");
    fm(0xa0, 0x41, opl);  /* 440 Hz */
    fm(0xb0, 0x32, opl);  /* 440 Hz, block 0, key on */

    SDL_Delay(1000);

    fm(0xb0, 0x12, opl);  /* key off */

    SDL_Delay(1000);
    // ---------------- left / right test

    // TODO: opl->isStereo();
    // TODO: opl->getType();
    // only opl3
    spdlog::info("Left/Right OPL3");
    int block = 4;        /* choose block=4 and m=1 */
    int m = 1;		       /* m is the frequency multiple number */
    int f = 440;          /* want f=440 Hz */
    int b = 1 << block;
    /* This is the equation to calculate frequency number from frequency. */
    int fn = (long)f * 1048576 / b / m / 50000L;
    constexpr int KEYON = 0x20;     // key-on bit in regs b0 - b8

    /* This left and right channel stuff is the only part of this program
     * that uses OPL3 mode.  Everything else is available on the OPL2.
     */
    fm(0xA0, (fn & 0xFF), opl);
    fm(0xB0, ((fn >> 8) & 0x3) + (block << 2) | KEYON, opl);
    SDL_Delay(1000);

    spdlog::info("Left channel only");
    fm(0xC0, LEFT | 1, opl);      /* set left channel only, parallel connection */
    SDL_Delay(1000);

    spdlog::info("Right channel only");
    fm(0xC0, RIGHT | 1, opl);     /* set right channel only, parallel connection */
    SDL_Delay(1000);

    fm(0xb0, 0x12, opl);  /* key off */
    Profm2(5, 0, opl);   /* set back to OPL2 mode */
}

int adl_driver_mame()
{
    Mix_Init(0);
    int rate = 22050;
    if (Mix_OpenAudio(rate, AUDIO_S16, 2, 1024) < 0) {
        cerr << Mix_GetError();
        return -1;
    }

    //spdlog::set_level(spdlog::level::debug);
    std::shared_ptr<audio::SDL2Mixer> mixer = std::make_shared<audio::SDL2Mixer>();
    //mixer->_rate = rate;
    std::shared_ptr<files::ADLFile> adlFile = std::make_shared<files::ADLFile>("DUNE0.ADL");
    std::shared_ptr<hardware::opl::scummvm::mame::OPL> opl = std::make_shared<hardware::opl::scummvm::mame::OPL>(mixer);
    drivers::westwood::ADLDriver adlDrv(opl, adlFile);

    adlDrv.startSound(2, 254);
    //TODO: SoundHandle ?
    Mix_VolumeMusic(MIX_MAX_VOLUME);
    Mix_HookMusic(&callback_mame, opl.get());
    SDL_Delay(20000);

    Mix_HaltChannel(-1);
    Mix_HaltMusic();
    Mix_CloseAudio();
    Mix_Quit();

    return 0;
}

int mame_opl_test()
{
    Mix_Init(0);
    int rate = 22050;
    if (Mix_OpenAudio(rate, AUDIO_S16, 2, 1024) < 0) {
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

    spdlog::set_level(spdlog::level::debug);
    std::shared_ptr<audio::SDL2Mixer> mixer = std::make_shared<audio::SDL2Mixer>();
    //mixer->_rate = rate;
    std::shared_ptr<hardware::opl::scummvm::mame::OPL> opl = std::make_shared<hardware::opl::scummvm::mame::OPL>(mixer);
    Mix_HookMusic(callback_mame, opl.get());
    opl2_test(opl);
    
    Mix_HaltChannel(-1);
    Mix_HaltMusic();
    Mix_CloseAudio();
    Mix_Quit();

    return 0;

}

int dosbox_opl2_test()
{
    Mix_Init(0);
    int rate = 22050;
    if (Mix_OpenAudio(rate, AUDIO_S16, 2, 1024) < 0) {
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

    spdlog::set_level(spdlog::level::debug);
    std::shared_ptr<audio::SDL2Mixer> mixer = std::make_shared<audio::SDL2Mixer>();
    std::shared_ptr<hardware::opl::scummvm::dosbox::OPL> opl = std::make_shared<hardware::opl::scummvm::dosbox::OPL>(mixer, hardware::opl::scummvm::Config::OplType::OPL2);
    opl2_test(opl);
    Mix_HookMusic(callback_dosbox, opl.get());

    SDL_Delay(10000);

    Mix_HaltChannel(-1);
    Mix_HaltMusic();
    Mix_CloseAudio();
    Mix_Quit();

    return 0;
}

int dosbox_dual_opl2_test()
{
    Mix_Init(0);
    int rate = 22050;
    if (Mix_OpenAudio(rate, AUDIO_S16, 2, 1024) < 0) {
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

    spdlog::set_level(spdlog::level::debug);
    std::shared_ptr<audio::SDL2Mixer> mixer = std::make_shared<audio::SDL2Mixer>();
    std::shared_ptr<hardware::opl::scummvm::dosbox::OPL> opl = std::make_shared<hardware::opl::scummvm::dosbox::OPL>(mixer, hardware::opl::scummvm::Config::OplType::DUAL_OPL2);
    Mix_HookMusic(callback_dosbox, opl.get());
    dual_opl2_test(opl);
   
    Mix_HaltChannel(-1);
    Mix_HaltMusic();
    Mix_CloseAudio();
    Mix_Quit();

    return 0;
}

int dosbox_opl3_test()
{
    Mix_Init(0);
    int rate = 22050;
    if (Mix_OpenAudio(rate, AUDIO_S16, 2, 1024) < 0) {
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

    spdlog::set_level(spdlog::level::debug);
    std::shared_ptr<audio::SDL2Mixer> mixer = std::make_shared<audio::SDL2Mixer>();
    std::shared_ptr<hardware::opl::scummvm::dosbox::OPL> opl = std::make_shared<hardware::opl::scummvm::dosbox::OPL>(mixer, hardware::opl::scummvm::Config::OplType::OPL3);
    Mix_HookMusic(callback_dosbox, opl.get());
    opl3_test(opl);

    Mix_HaltChannel(-1);
    Mix_HaltMusic();
    Mix_CloseAudio();
    Mix_Quit();

    return 0;
}

int nuked_opl2_test()
{
    Mix_Init(0);
    int rate = 22050;
    if (Mix_OpenAudio(rate, AUDIO_S16, 2, 1024) < 0) {
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

    spdlog::set_level(spdlog::level::debug);
    std::shared_ptr<audio::SDL2Mixer> mixer = std::make_shared<audio::SDL2Mixer>();
    std::shared_ptr<hardware::opl::scummvm::nuked::OPL> opl = std::make_shared<hardware::opl::scummvm::nuked::OPL>(mixer, hardware::opl::scummvm::Config::OplType::OPL2);
    opl2_test(opl);
    Mix_HookMusic(callback_nuked, opl.get());

    SDL_Delay(10000);

    Mix_HaltChannel(-1);
    Mix_HaltMusic();
    Mix_CloseAudio();
    Mix_Quit();

    return 0;
}

int nuked_dual_opl2_test()
{
    Mix_Init(0);
    int rate = 22050;
    if (Mix_OpenAudio(rate, AUDIO_S16, 2, 1024) < 0) {
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

    spdlog::set_level(spdlog::level::debug);
    std::shared_ptr<audio::SDL2Mixer> mixer = std::make_shared<audio::SDL2Mixer>();
    std::shared_ptr<hardware::opl::scummvm::nuked::OPL> opl = std::make_shared<hardware::opl::scummvm::nuked::OPL>(mixer, hardware::opl::scummvm::Config::OplType::DUAL_OPL2);
    Mix_HookMusic(callback_nuked, opl.get());
    dual_opl2_test(opl);

    Mix_HaltChannel(-1);
    Mix_HaltMusic();
    Mix_CloseAudio();
    Mix_Quit();

    return 0;
}

int nuked_opl3_test()
{
    Mix_Init(0);
    int rate = 22050;
    if (Mix_OpenAudio(rate, AUDIO_S16, 2, 1024) < 0) {
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

    spdlog::set_level(spdlog::level::debug);
    std::shared_ptr<audio::SDL2Mixer> mixer = std::make_shared<audio::SDL2Mixer>();
    std::shared_ptr<hardware::opl::scummvm::nuked::OPL> opl = std::make_shared<hardware::opl::scummvm::nuked::OPL>(mixer, hardware::opl::scummvm::Config::OplType::OPL3);
    Mix_HookMusic(callback_nuked, opl.get());
    opl3_test(opl);

    Mix_HaltChannel(-1);
    Mix_HaltMusic();
    Mix_CloseAudio();
    Mix_Quit();

    return 0;
}

int surround_dual_opl2_test()
{
    Mix_Init(0);
    int rate = 22050;
    if (Mix_OpenAudio(rate, AUDIO_S16, 2, 1024) < 0) {
        cerr << Mix_GetError();
        return -1;
    }

    spdlog::set_level(spdlog::level::debug);
    std::shared_ptr<audio::SDL2Mixer> mixer = std::make_shared<audio::SDL2Mixer>();
    std::shared_ptr<hardware::opl::woody::SurroundOPL> opl = std::make_shared<hardware::opl::woody::SurroundOPL>(mixer);
    Mix_HookMusic(callback_surround, opl.get());
    dual_opl2_test(opl);

    Mix_HaltChannel(-1);
    Mix_HaltMusic();
    Mix_CloseAudio();
    Mix_Quit();

    return 0;
}


int adl_driver_dosbox()
{
    Mix_Init(0);
    int rate = 22050;
    if (Mix_OpenAudio(rate, AUDIO_S16, 2, 1024) < 0) {
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

    //spdlog::set_level(spdlog::level::debug);
    std::shared_ptr<audio::SDL2Mixer> mixer = std::make_shared<audio::SDL2Mixer>();
    std::shared_ptr<files::ADLFile> adlFile = std::make_shared<files::ADLFile>("DUNE0.ADL");
    std::shared_ptr<hardware::opl::scummvm::dosbox::OPL> opl = std::make_shared<hardware::opl::scummvm::dosbox::OPL>(mixer, hardware::opl::scummvm::Config::OplType::OPL3);
    drivers::westwood::ADLDriver adlDrv(opl, adlFile);
    
    adlDrv.initDriver();

    // TODO: ADLFile get track first value should be 9, instead return 0
    adlDrv.startSound(4, 128);
    //TODO: SoundHandle ?
    Mix_VolumeMusic(MIX_MAX_VOLUME);
    Mix_HookMusic(&callback_dosbox, opl.get());
    SDL_Delay(60000);


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
        
    //adl();
    //adl_driver_woody();
    //adl_driver_mame();
    //mame_opl_test();
    //dosbox_opl2_test();
    //dosbox_dual_opl2_test();
    //dosbox_opl3_test();
    //nuked_opl2_test();
    //nuked_dual_opl2_test();
    //nuked_opl3_test();
    surround_dual_opl2_test();
    //adl_driver_dosbox();

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
