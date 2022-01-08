#include <hardware/opl/OPL.hpp>
#include <hardware/opl/scummvm/Config.hpp>
#include <utils/algorithms.hpp>
#include <audio/scummvm/SDLMixerManager.hpp>
#include <files/westwood/ADLFile.hpp>
#include <drivers/westwood/ADLDriver.hpp>

#include <spdlog/spdlog.h>
#include <fmt/color.h>

#include <memory>
#include <cstdint>
#include <map>
#include <string>


using audio::scummvm::SdlMixerManager;
using hardware::opl::scummvm::Config;
using hardware::opl::scummvm::OplEmulator;
using utils::delayMillis;
using files::westwood::ADLFile;
using drivers::westwood::ADLDriver;


void adl_test(const OplEmulator emu, const Config::OplType type, std::shared_ptr<audio::scummvm::Mixer> mixer, const std::string& filename, const int track)
{
    //spdlog::set_level(spdlog::level::debug);
    auto opl = Config::create(emu, type, mixer);
    if (opl == nullptr)
        return;

    auto adlFile = std::make_shared<ADLFile>(filename);
    ADLDriver adlDrv(opl, adlFile);
    adlDrv.play(track, 0xFF);

    while (!mixer->isReady()) {
        spdlog::info("mixer not ready yet..");
        delayMillis(100);
    }

    do
    {
        //spdlog::info("is playing");
        delayMillis(1000);

    } while (adlDrv.isPlaying());
}

int main(int argc, char* argv[])
{
    SdlMixerManager mixerManager;

    mixerManager.init();

    auto mixer = mixerManager.getMixer();

    std::map<OplEmulator, std::string> emus = {
        { OplEmulator::DOS_BOX, "DOS_BOX" },
        { OplEmulator::MAME, "MAME" },
        { OplEmulator::NUKED, "NUKED" },
        { OplEmulator::WOODY, "WOODY" },
    };

    std::map<Config::OplType, std::string> types = {
        {Config::OplType::OPL2, "OPL2"},
        {Config::OplType::DUAL_OPL2, "DUAL_OPL2"},
        {Config::OplType::OPL3, "OPL3"},
    };

    std::string m = "##### {} {} #####";
    for (auto& emu : emus)
    {
        for (auto& type : types)
        {
            for (auto& c : { fmt::color::white_smoke, fmt::color::yellow,      fmt::color::aqua,
                             fmt::color::lime_green,  fmt::color::blue_violet, fmt::color::indian_red }) {
                spdlog::info(fmt::format(fg(c), m, emu.second, type.second));
            }

            adl_test(emu.first, type.first, mixer, "DUNE0.ADL", 4);
            //adl_test(emu.first, type.first, mixer, "EOBSOUND.ADL", 1);
            //adl_test(emu.first, type.first, mixer, "LOREINTR.ADL", 3);
        }
    }

    return 0;
}
