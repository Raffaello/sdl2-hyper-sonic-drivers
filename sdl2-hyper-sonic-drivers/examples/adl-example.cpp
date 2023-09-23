#include <HyperSonicDrivers/audio/sdl2/Mixer.hpp>
#include <HyperSonicDrivers/hardware/opl/OPL.hpp>
#include <HyperSonicDrivers/hardware/opl/OPLFactory.hpp>
#include <HyperSonicDrivers/utils/algorithms.hpp>
#include <HyperSonicDrivers/files/westwood/ADLFile.hpp>
#include <HyperSonicDrivers/drivers/westwood/ADLDriver.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>
#include <HyperSonicDrivers/devices/Adlib.hpp>
#include <HyperSonicDrivers/devices/SbPro.hpp>
#include <HyperSonicDrivers/devices/SbPro2.hpp>

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
    using devices::make_device;

    auto adlFile = std::make_shared<ADLFile>(filename);
    std::shared_ptr<devices::Opl> device;
    switch (type)
    {
        using enum OplType;

    case OPL2:
        device = make_device<devices::Adlib, devices::Opl>(mixer, emu);
        break;
    case DUAL_OPL2:
        device = make_device<devices::SbPro, devices::Opl>(mixer, emu);
        break;
    case OPL3:
        device = make_device<devices::SbPro2, devices::Opl>(mixer, emu);
        break;

    }

    ADLDriver adlDrv(device, audio::mixer::eChannelGroup::Music);
    adlDrv.setADLFile(adlFile);
    adlDrv.play(track);

    if(!mixer->isReady()) {
        spdlog::error("mixer not ready yet..");
        return;
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

    const std::map<OplEmulator, std::string> emus = {
        { OplEmulator::DOS_BOX, "DOS_BOX" },
        { OplEmulator::MAME, "MAME" },
        { OplEmulator::NUKED, "NUKED" },
        { OplEmulator::WOODY, "WOODY" },
    };

    const std::map<OplType, std::string> types = {
        {OplType::OPL2, "OPL2"},
        {OplType::DUAL_OPL2, "DUAL_OPL2"},
        {OplType::OPL3, "OPL3"},
    };

    const std::string m = "##### {} {} #####";

    spdlog::set_level(spdlog::level::info);
    HyperSonicDrivers::utils::ILogger::instance->setLevelAll(HyperSonicDrivers::utils::ILogger::eLevel::Info);
    for (const auto& emu : emus)
    {
        for (const auto& type : types)
        {
            using enum fmt::color;

            for (const auto& c : { white_smoke, yellow,      aqua,
                             lime_green,  blue_violet, indian_red }) {
                spdlog::info(fmt::format(fg(c), m, emu.second, type.second));
            }

            try
            {
                adl_test(emu.first, type.first, mixer, "DUNE0.ADL", 4);
                //adl_test(emu.first, type.first, mixer, "EOBSOUND.ADL", 1);
                //adl_test(emu.first, type.first, mixer, "LOREINTR.ADL", 3);
            }
            catch (const std::exception& e)
            {
                spdlog::default_logger()->error(e.what());
            }
        }
    }

    return 0;
}
