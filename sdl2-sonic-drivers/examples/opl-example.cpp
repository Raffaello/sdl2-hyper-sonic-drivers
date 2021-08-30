#include <hardware/opl/OPL.hpp>
#include <hardware/opl/scummvm/EmulatedOPL.hpp>
#include <hardware/opl/scummvm/Config.hpp>
#include <utils/algorithms.hpp>
#include <audio/SDL2Mixer.hpp>
#include <audio/scummvm/SDLMixerManager.hpp>

#include <spdlog/spdlog.h>
#include <memory>
#include <cstdint>

#include <SDL2/SDL.h> // TODO: do a delay utils


using audio::scummvm::SdlMixerManager;
using audio::SDL2Mixer;
using hardware::opl::scummvm::Config;
using hardware::opl::scummvm::OplEmulator;

void callback_sdl(void* userdata, uint8_t* stream, int len)
{
    hardware::opl::scummvm::EmulatedOPL* opl = reinterpret_cast<hardware::opl::scummvm::EmulatedOPL*>(userdata);
    int16_t* buf = reinterpret_cast<int16_t*>(stream);
    
    // 16 bits
    const int l = len >> 1;
    memset(buf, 0, len);
    int samples = opl->readBuffer(buf, l);
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

    spdlog::info("440 Hz tone, values looked up in table.\n");
    fm(0xa0, 0x41, opl);  /* 440 Hz */
    fm(0xb0, 0x32, opl);  /* 440 Hz, block 0, key on */

    SDL_Delay(1000);

    fm(0xb0, 0x12, opl);  /* key off */
}

void dual_opl2_test(std::shared_ptr<hardware::opl::OPL> opl)
{
    opl->init();
    //opl->setCallbackFrequency(72);

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


int sdl()
{
    SDL_Init(SDL_INIT_AUDIO);

    int numAudioDevices = SDL_GetNumAudioDevices(0);
    spdlog::info("numAudioDevices = {}", numAudioDevices);

    for (int i = 0; i < numAudioDevices; i++) {
        spdlog::info("Device {} name: {}", i, SDL_GetAudioDeviceName(i, 0));
    }

    int numAudioDrivers = SDL_GetNumAudioDrivers();
    spdlog::info("numAudioDrivers = {}", numAudioDrivers);
    for (int i = 0; i < numAudioDrivers; i++) {
        spdlog::info("Driver {} name: {}", i, SDL_GetAudioDriver(i));
    }

    SDL_AudioSpec desired = {};
    desired.callback = callback_sdl;
    desired.channels = 2;
    desired.format = AUDIO_S16;
    desired.freq = 22050;
    desired.samples = 1024;
    desired.silence = 0;

    auto mixer = std::make_shared<audio::SDL2Mixer>(desired);
    auto opl = Config::create(OplEmulator::NUKED, Config::OplType::OPL2, mixer);

    desired.userdata = opl.get();

    //if (SDL_OpenAudioDevice(NULL, 0, &desired, &obtained, SDL_AUDIO_ALLOW_ANY_CHANGE) == 0) {
    if (SDL_OpenAudio(&desired, NULL) == -1) {
        spdlog::critical("unble to open audio device: {}", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    //spdlog::set_level(spdlog::level::debug);
    spdlog::info("channels {} --- freq {} ---  format {:#06x} ", desired.channels, desired.freq, desired.format);

    SDL_PauseAudio(0);

    opl2_test(opl);

    SDL_PauseAudio(1);
    SDL_CloseAudio();

    SDL_Quit();

    return 0;
}

/*
int mix()
{
    Mix_Init(0);
    int rate = 22050;
    if (Mix_OpenAudio(rate, AUDIO_S16, 2, 1024) < 0) {
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
        cerr << "CHANNELS not mono or stereo!" << endl;
    }

    spdlog::set_level(spdlog::level::debug);
    std::shared_ptr<audio::SDL2Mixer> mixer = std::make_shared<audio::SDL2Mixer>();
    //std::shared_ptr<hardware::opl::scummvm::mame::OPL> opl = std::make_shared<hardware::opl::scummvm::mame::OPL>(mixer);
    auto opl = Config::create(OplEmulator::AUTO, Config::OplType::OPL2, mixer);
    Mix_HookMusic(callback_sdl, opl.get());
    opl2_test(opl);

    Mix_HaltChannel(-1);
    Mix_HaltMusic();
    Mix_CloseAudio();
    Mix_Quit();

    return 0;
}
*/

int sdlMM()
{
    SdlMixerManager mixerManager;

    mixerManager.init();

    auto mixer = mixerManager.getMixer();

    auto opl = Config::create(OplEmulator::DOS_BOX, Config::OplType::OPL2, mixer);

    opl->start(nullptr);
    opl2_test(opl);
    
    return 0;
}

int main(int argc, char* argv[])
{
    return sdlMM();

    //return sdl();

    //return mix();
}
