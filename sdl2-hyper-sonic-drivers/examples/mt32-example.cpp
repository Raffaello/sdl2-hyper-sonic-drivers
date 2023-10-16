#include <HyperSonicDrivers/audio/sdl2/Mixer.hpp>
#include <HyperSonicDrivers/drivers/MIDDriver.hpp>
#include <HyperSonicDrivers/files/MIDFile.hpp>
#include <HyperSonicDrivers/devices/MT32.hpp>
#include <HyperSonicDrivers/utils/algorithms.hpp>

#include <memory>
#include <spdlog/spdlog.h>
#include <fmt/chrono.h>
#include <fmt/color.h>
#include <map>

using namespace HyperSonicDrivers;

int main(int argc, char* argv[])
{
    using audio::mixer::eChannelGroup;

    spdlog::warn("MT32 requires 2 ROMs files not included in this example");
    spdlog::warn("the ROM filenames assumed to be fund in the current working directory are:");
    spdlog::warn("MT32_CONTROL.ROM  --- MT32_PCM.ROM");

    auto mixer = audio::make_mixer<audio::sdl2::Mixer>(8, 44100, 1024);
    if (!mixer->init())
    {
        spdlog::error("can't init mixer");
        return 1;
    }

    auto mt32 = devices::make_device<devices::MT32>(mixer, "MT32_CONTROL.ROM", "MT32_PCM.ROM");

    if (!mt32->init())
    {
        spdlog::error("can't init MT32 device");
        return 2;
    }

    auto midFile = std::make_shared<files::MIDFile>("MI_intro.mid");
    auto midi = midFile->getMIDI();

    drivers::MIDDriver middrv(mt32, eChannelGroup::Music);
    middrv.setMidi(midi);
    middrv.play(0);
    while (middrv.isPlaying())
    {
        utils::delayMillis(100);
    }
}
