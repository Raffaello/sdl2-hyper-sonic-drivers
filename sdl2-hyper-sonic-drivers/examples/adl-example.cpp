#include <HyperSonicDrivers/audio/sdl2/Mixer.hpp>
#include <HyperSonicDrivers/hardware/opl/OPL.hpp>
#include <HyperSonicDrivers/hardware/opl/OPLFactory.hpp>
#include <HyperSonicDrivers/utils/algorithms.hpp>
#include <HyperSonicDrivers/files/westwood/ADLFile.hpp>
#include <HyperSonicDrivers/drivers/westwood/ADLDriver.hpp>

#include <spdlog/spdlog.h>
#include <fmt/color.h>

#include <memory>
#include <cstdint>
#include <map>
#include <string>

using namespace HyperSonicDrivers;

using hardware::opl::OPLFactory;
using hardware::opl::OplEmulator;
using hardware::opl::OplType;
using utils::delayMillis;
using files::westwood::ADLFile;
using drivers::westwood::ADLDriver;


void adl_test(const OplEmulator emu, const OplType type, std::shared_ptr<audio::IMixer> mixer, const std::string& filename, const int track)
{
    //spdlog::set_level(spdlog::level::debug);
    auto opl = OPLFactory::create(emu, type, mixer);
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
    auto mixer = audio::make_mixer<audio::sdl2::Mixer>(8, 44100, 1024);
    if (!mixer->init())
    {
        spdlog::error("can't init mixer");
        return 1;
    }

    std::map<OplEmulator, std::string> emus = {
        { OplEmulator::DOS_BOX, "DOS_BOX" },
        { OplEmulator::MAME, "MAME" },
        { OplEmulator::NUKED, "NUKED" },
        { OplEmulator::WOODY, "WOODY" },
    };

    std::map<OplType, std::string> types = {
        {OplType::OPL2, "OPL2"},
        {OplType::DUAL_OPL2, "DUAL_OPL2"},
        {OplType::OPL3, "OPL3"},
    };

    std::string m = "##### {} {} #####";
    for (auto& emu : emus)
    {
        for (auto& type : types)
        {
            using enum fmt::color;

            for (auto& c : { white_smoke, yellow,      aqua,
                             lime_green,  blue_violet, indian_red }) {
                spdlog::info(fmt::format(fg(c), m, emu.second, type.second));
            }

            adl_test(emu.first, type.first, mixer, "DUNE0.ADL", 4);
            //adl_test(emu.first, type.first, mixer, "EOBSOUND.ADL", 1);
            //adl_test(emu.first, type.first, mixer, "LOREINTR.ADL", 3);
        }
    }

    return 0;
}
