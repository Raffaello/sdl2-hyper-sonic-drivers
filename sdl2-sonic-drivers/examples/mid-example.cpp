#include <audio/scummvm/Mixer.hpp>
#include <audio/scummvm/SDLMixerManager.hpp>
#include <files/MIDFile.hpp>
#include <drivers/MIDDriver.hpp>
#include <drivers/midi/DeviceNative.hpp>

#include <spdlog/spdlog.h>
#include <fmt/chrono.h>

int main(int argc, char* argv[])
{
    audio::scummvm::SdlMixerManager mixerManager;
    mixerManager.init();

    auto mixer = mixerManager.getMixer();

    // Reproducing MIDI file
    auto midFile = std::make_shared<files::MIDFile>("MI_intro.mid");
    //auto midFile = std::make_shared<files::MIDFile>("midifile_sample.mid");
    auto midi = midFile->convertToSingleTrackMIDI();
    auto nativeMidi = std::make_shared<drivers::midi::DeviceNative>();
    
    drivers::MIDDriver mid_drv(mixer, nativeMidi);
    auto start_time = std::chrono::system_clock::now();
    //spdlog::set_level(spdlog::level::debug);
    mid_drv.play(midi);
    
    //while (mid_drv.isPlayng()) {
    //    utils::
    //}
    
    spdlog::info("is Playing: ???");
    
    auto end_time = std::chrono::system_clock::now();
    auto tot_time = end_time - start_time;
    spdlog::info("Total Running Time: {:%M:%S}", tot_time);

    return 0;
}

 


