﻿// sdl2-sonic-drivers.cpp : Defines the entry point for the application.
//

#include <iostream>

#include <SDL2/SDL.h>

#include <audio/scummvm/SDLMixerManager.hpp>
#include <drivers/westwood/ADLDriver.hpp>
#include <files/miles/XMIFile.hpp>
#include <files/westwood/ADLFile.hpp>
#include <hardware/PCSpeaker.hpp>
#include <hardware/opl/scummvm/Config.hpp>

#include <hardware/opl/mame/MameOPL.hpp>

#include <audio/DiskRendererMixerManager.hpp>

#include <files/MIDFile.hpp>
#include <drivers/miles/XMIParser.hpp>

#include <utils/algorithms.hpp>

#include <spdlog/spdlog.h>


#include <drivers/midi/devices/ScummVM.hpp>
#include <drivers/MIDDriver.hpp>

#include <files/dmx/MUSFile.hpp>
#include <files/dmx/OP2File.hpp>
#include <drivers/midi/devices/Native.hpp>

#include <drivers/midi/devices/Adlib.hpp>


using namespace std;

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

int renderMixer()
{
    using namespace audio::scummvm;
    using namespace hardware::opl::scummvm;
    using namespace drivers::westwood;

    audio::DiskRendererMixerManager mixerManager(44100);
    mixerManager.init();
    mixerManager.startRecording("test.dat");

    std::shared_ptr<Mixer> mixer = mixerManager.getMixer();

    //spdlog::set_level(spdlog::level::debug);
    auto opl = Config::create(OplEmulator::NUKED, Config::OplType::OPL3, mixer);
    auto pOpl = dynamic_cast<EmulatedOPL*>( opl.get());
    //auto opl = std::make_shared<hardware::opl::mame::MameOPL>(mixer);
    std::shared_ptr<files::westwood::ADLFile> adlFile = std::make_shared<files::westwood::ADLFile>("test/fixtures/DUNE0.ADL");

    ADLDriver adlDrv(opl, adlFile);
    adlDrv.play(4, 0xFF);
    int samples = -1;
    int totSamples = 0;
    bool isPlaying = adlDrv.isPlaying();
    do
    {
        // TODO review, but is dumping the data
        int16_t buf[1024];

        samples = pOpl->readBuffer(buf, 1024);
        mixerManager.callbackHandler((uint8_t*)buf, samples * 2);
        totSamples += samples;
        isPlaying = adlDrv.isPlaying();
        //spdlog::info("isPlaying? {}", isPlaying);
    } while (isPlaying);

    spdlog::info("TotSamples={} --- space require={} ({}KB) [{}MB]", totSamples, totSamples * sizeof(int16_t), totSamples * sizeof(int16_t) / 1024, totSamples * sizeof(int16_t) / 1024 / 1024);

    while (!mixer->isReady()) {
        spdlog::info("mixer not ready");
        utils::delayMillis(100);
    }

    utils::delayMillis(1000);
    while (adlDrv.isPlaying())
    {
        spdlog::info("is playing");
        utils::delayMillis(100);

    }

    spdlog::info("renderer quitting...");

    return 0;
}

int xmi_parser()
{
    using namespace audio::scummvm;
    using  drivers::miles::XMIParser;

    SdlMixerManager mixerManager;
    mixerManager.init();

    std::shared_ptr<Mixer> mixer = mixerManager.getMixer();

    //spdlog::set_level(spdlog::level::debug);
    std::shared_ptr<files::miles::XMIFile> xmiFile = std::make_shared<files::miles::XMIFile>("test/fixtures/AIL2_14_DEMO.XMI");

    XMIParser xmiParser(xmiFile->getMIDI(), mixer);
    xmiParser.displayAllTracks();



    spdlog::info("SDLMixer quitting...");
    SDL_Delay(1000);
    spdlog::info("SDLMixer quit");

    return 0;

}

int midi_adlib()
{
    using namespace audio::scummvm;
    using hardware::opl::scummvm::Config;
    using hardware::opl::scummvm::OplEmulator;

    SdlMixerManager mixerManager;
    mixerManager.init();

    std::shared_ptr<Mixer> mixer = mixerManager.getMixer();

    auto emu = OplEmulator::NUKED;
    auto type = Config::OplType::OPL3;
    
    auto opl = Config::create(emu, type, mixer);
    if (opl.get() == nullptr)
        return -1;

    //spdlog::set_level(spdlog::level::debug);
    //std::shared_ptr<files::MIDFile> midFile = std::make_shared<files::MIDFile>("test/fixtures/MI_intro.mid");
    auto midFile = std::make_shared<files::MIDFile>("test/fixtures/midifile_sample.mid");
    auto midi = midFile->convertToSingleTrackMIDI();
    auto scumm_midi = std::make_shared<drivers::midi::devices::ScummVM>(opl, true);
    drivers::MIDDriver midDrv(mixer, scumm_midi);


    spdlog::info("playing midi...");
    midDrv.play(midi);
    spdlog::info("end.");

    return 0;
}

int midi_adlib_mus_file_CONCURRENCY_ERROR_ON_SAME_DEVICE()
{
    using namespace audio::scummvm;
    using hardware::opl::scummvm::Config;
    using hardware::opl::scummvm::OplEmulator;

    SdlMixerManager mixerManager;
    mixerManager.init();

    std::shared_ptr<Mixer> mixer = mixerManager.getMixer();

    auto emu = OplEmulator::MAME;
    auto type = Config::OplType::OPL2;

    auto opl = Config::create(emu, type, mixer);
    if (opl.get() == nullptr)
        return -1;

    //spdlog::set_level(spdlog::level::debug);
    auto midFile = std::make_shared<files::MIDFile>("test/fixtures/MI_intro.mid");
    auto musFile = std::make_shared<files::dmx::MUSFile>("test/fixtures/D_E1M1.MUS");
    auto midi = musFile->getMIDI();
    auto scumm_midi = std::make_shared<drivers::midi::devices::ScummVM>(opl, false);
    drivers::MIDDriver midDrv(mixer, scumm_midi);
    drivers::MIDDriver midDrv2(mixer, scumm_midi);

    spdlog::info("playing midi D_E1M1.MUS...");
    midDrv.play(midi);
    utils::delayMillis(9000);
    spdlog::info("playing midi2 D_E1M1.MUS... (this should not be possible with the same device)");
    midDrv2.play(midFile->convertToSingleTrackMIDI());
    spdlog::info("end.");
    while (midDrv.isPlaying() || midDrv2.isPlaying())
    {
        utils::delayMillis(1000);
    }

    return 0;
}

template <> struct fmt::formatter<files::dmx::OP2File::instrument_voice_t> {
    char presentation = 'f';
    // Parses format specifications of the form ['f' | 'e'].
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        // [ctx.begin(), ctx.end()) is a character range that contains a part of
        // the format string starting from the format specifications to be parsed,
        // e.g. in
        //
        //   fmt::format("{:f} - point of interest", point{1, 2});
        //
        // the range will contain "f} - point of interest". The formatter should
        // parse specifiers until '}' or the end of the range. In this example
        // the formatter should parse the 'f' specifier and return an iterator
        // pointing to '}'.

        // Please also note that this character range may be empty, in case of
        // the "{}" format string, so therefore you should check ctx.begin()
        // for equality with ctx.end().

        // Parse the presentation format and store it in the formatter:
        auto it = ctx.begin(), end = ctx.end();
        if (it != end && (*it == 'f' || *it == 'e')) presentation = *it++;

        // Check if reached the end of the range:
        if (it != end && *it != '}') throw format_error("invalid format");

        // Return an iterator past the end of the parsed range:
        return it;
    }

    // Formats the point p using the parsed format specification (presentation)
  // stored in this formatter.
    template <typename FormatContext>
    auto format(const files::dmx::OP2File::instrument_voice_t& voice, FormatContext& ctx) const -> decltype(ctx.out()) {
        // ctx.out() is an output iterator to write to.
        return presentation == 'f'
            ? fmt::format_to(ctx.out(), "({:x})", voice.iModAttack)
            : fmt::format_to(ctx.out(), "use :f format type to dispaly it");
    }
};

template <> struct fmt::formatter<files::dmx::OP2File::instrument_t> {
    char presentation = 'f';
    // Parses format specifications of the form ['f' | 'e'].
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        // [ctx.begin(), ctx.end()) is a character range that contains a part of
        // the format string starting from the format specifications to be parsed,
        // e.g. in
        //
        //   fmt::format("{:f} - point of interest", point{1, 2});
        //
        // the range will contain "f} - point of interest". The formatter should
        // parse specifiers until '}' or the end of the range. In this example
        // the formatter should parse the 'f' specifier and return an iterator
        // pointing to '}'.

        // Please also note that this character range may be empty, in case of
        // the "{}" format string, so therefore you should check ctx.begin()
        // for equality with ctx.end().

        // Parse the presentation format and store it in the formatter:
        auto it = ctx.begin(), end = ctx.end();
        if (it != end && (*it == 'f' || *it == 'e')) presentation = *it++;

        // Check if reached the end of the range:
        if (it != end && *it != '}') throw format_error("invalid format");

        // Return an iterator past the end of the parsed range:
        return it;
    }

    // Formats the point p using the parsed format specification (presentation)
  // stored in this formatter.
    template <typename FormatContext>
    auto format(const files::dmx::OP2File::instrument_t& inst, FormatContext& ctx) const -> decltype(ctx.out()) {
        // ctx.out() is an output iterator to write to.
        return presentation == 'f'
            ? fmt::format_to(ctx.out(), "(flags: {:x}, noteNum: {:d}, fineTune: {:d}, voicesSize: {:d}) -- voice1: {:f}", inst.flags, inst.noteNum, inst.fineTune, inst.voices.size(),inst.voices[0])
            : fmt::format_to(ctx.out(), "use :f format type to dispaly it");
    }
};

int midi_adlib_mus_file_genmidi()
{
    using namespace audio::scummvm;
    using hardware::opl::scummvm::Config;
    using hardware::opl::scummvm::OplEmulator;

    SdlMixerManager mixerManager;
    mixerManager.init();

    std::shared_ptr<Mixer> mixer = mixerManager.getMixer();

    auto emu = OplEmulator::MAME;
    auto type = Config::OplType::OPL2;

    auto opl = Config::create(emu, type, mixer);
    if (opl.get() == nullptr)
        return -1;

    auto op2File = std::make_shared<files::dmx::OP2File>("test/fixtures/GENMIDI.OP2");
    auto musFile = std::make_shared<files::dmx::MUSFile>("test/fixtures/D_E1M1.MUS");

    // not supported but working ok without op2file (wrong instrument sounds though)
    auto midi = musFile->getMIDI(/*op2File*/);
    // TODO: create a new device instead of ScummVM like Adlip or OPL (opl can be both 2 or 3, adlib only 2)
    // TODO: start with doing Adlib? than eventually refactor in a general OPL etc..
    auto adlib_midi = std::make_shared<drivers::midi::devices::Adlib>(opl);
    drivers::MIDDriver midDrv(mixer, adlib_midi);
    //auto native_midi = std::make_shared<drivers::midi::devices::Native>();
    //drivers::MIDDriver midDrv(mixer, native_midi);
    std::array<uint8_t, 4> instrument_values = { 30,29,34,0 };
    for (int i = 0; i < instrument_values.size(); i++) {
        int j = instrument_values[i];

        spdlog::info("{:d} instr name: {}", j, op2File->getInstrumentName(j));
        auto adlibInstr1 = op2File->getInstrumentToAdlib(j);
        spdlog::info("{:d} instr vars: {:f}", j, op2File->getInstrument(j));
    }
    spdlog::set_level(spdlog::level::debug);
    spdlog::info("instr name: {}", op2File->getInstrumentName(29));
    spdlog::info("instr vars: {:f}", op2File->getInstrument(29));
    
    spdlog::info("playing midi D_E1M1.MUS...");
    midDrv.play(midi);
    while (midDrv.isPlaying())
    {
        utils::delayMillis(1000);
    }

    return 0;
}


int midi_adlib_xmi()
{
    // Working but in a reduced way as there are specific XMI midi messages not interpreted
    // also it has been hacked through the xmifile get midi to build a single track midi

    using namespace audio::scummvm;
    using hardware::opl::scummvm::Config;
    using hardware::opl::scummvm::OplEmulator;

    SdlMixerManager mixerManager;
    mixerManager.init();

    std::shared_ptr<Mixer> mixer = mixerManager.getMixer();

    auto emu = OplEmulator::MAME;
    auto type = Config::OplType::OPL2;

    auto opl = Config::create(emu, type, mixer);
    if (opl.get() == nullptr)
        return -1;

    //spdlog::set_level(spdlog::level::debug);
    std::shared_ptr<files::miles::XMIFile> xmiFile = std::make_shared<files::miles::XMIFile>("test/fixtures/AIL2_14_DEMO.XMI");
    auto m = xmiFile->getMIDI();
    auto midi = std::make_shared<audio::MIDI>(audio::midi::MIDI_FORMAT::SINGLE_TRACK, 1, m->division);
    midi->addTrack(m->getTrack(0));
    
    auto scumm_midi = std::make_shared<drivers::midi::devices::ScummVM>(opl, false);
    drivers::MIDDriver midDrv(mixer, scumm_midi);

    spdlog::info("playing midi AIL2_14_DEMO...");
    midDrv.play(midi);
    
    while (midDrv.isPlaying())
    {
        utils::delayMillis(1000);
    }
    return 0;
}



int main(int argc, char* argv[])
{
    //sdlMixer();
    //SDL_Delay(100);
    //renderMixer();

    //xmi_parser();
    //midi_adlib_mus_file_CONCURRENCY_ERROR_ON_SAME_DEVICE()
    midi_adlib_mus_file_genmidi();
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
