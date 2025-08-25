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

#include <SDL2/SDL.h>

#include <memory>
#include <cstdint>
#include <map>
#include <string>

using namespace HyperSonicDrivers;

using drivers::westwood::ADLDriver;
using files::westwood::ADLFile;
using hardware::opl::OplEmulator;
using hardware::opl::OPLFactory;
using hardware::opl::OplType;
using utils::delayMillis;

/**
 * @brief Play an ADL file using a chosen OPL emulator/type until the user exits.
 *
 * Plays the given ADL file through an OPL device constructed from the specified
 * emulator and OPL type. The function creates an ADLDriver bound to the
 * Music channel group and enters an SDL event-driven loop that starts playback
 * (initial track index is 5) and responds to key presses to control playback.
 *
 * Behavior:
 * - Selects an OPL device based on `type`:
 *   - OPL2     -> Adlib + Opl
 *   - DUAL_OPL2-> SbPro  + Opl
 *   - OPL3     -> SbPro2 + Opl
 * - If the provided mixer is not ready, logs an error and returns immediately.
 * - Starts playing the current track when not already playing.
 * - Key controls (during playback):
 *   - ESC: stop all channels and return (exit function)
 *   - RIGHT: stop channels and advance to the next track (wraps to 0)
 *   - LEFT: stop channels and go to the previous track (wraps to last)
 *
 * This function blocks until ESC is pressed.
 *
 * @param emu  OPL emulator selection used when constructing the device.
 * @param type OPL hardware type used to choose the concrete device implementation.
 * @param filename Path to the ADL file to load and play.
 */
void adl_play(const OplEmulator emu, const OplType type, std::shared_ptr<audio::IMixer> mixer, const std::string &filename)
{
    using devices::make_device;
    using utils::ILogger;

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

    uint8_t track = 0;

    ADLDriver adlDrv(device, audio::mixer::eChannelGroup::Music);
    adlDrv.setADLFile(adlFile);

    if (!mixer->isReady())
    {
        spdlog::error("mixer not ready yet..");
        return;
    }

    do
    {
        if (!adlDrv.isPlaying())
        {
            adlDrv.play(track);
            ILogger::instance->info(fmt::format("Playing track: {}/{}", static_cast<int>(track), adlFile->getNumTracks()), ILogger::eCategory::Application);
        }
        // delayMillis(1000);
        SDL_Event e;
        while (SDL_WaitEventTimeout(&e, 100))
        {
            if (e.type == SDL_QUIT)
            {
                adlDrv.stopAllChannels();
                return;
            }
            else if (e.type == SDL_KEYDOWN)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                {
                    adlDrv.stopAllChannels();
                    return;
                }
                case SDLK_RIGHT:
                {
                    adlDrv.stopAllChannels();
                    track++;
                    if (track >= adlFile->getNumTracks())
                        track = 0;

                    break;
                }
                case SDLK_LEFT:
                {
                    adlDrv.stopAllChannels();
                    if (track > 0)
                        track--;
                    else
                        track = adlFile->getNumTracks() - 1;

                    break;
                }
                }
            }
        }

    } while (true);
}

/**
 * @brief Program entry point for the ADL playback demo using SDL2 and OPL emulation.
 *
 * Initializes SDL video, creates a minimal SDL window, and starts an SDL-based audio mixer.
 * Iterates over configured OPL emulator and OPL type combinations, prints a colored header
 * for each pair, and invokes adl_play to run the ADL playback demo for "DUNE0.ADL".
 * Cleans up SDL resources before exiting.
 *
 * Return codes:
 *  -  0 : Normal exit after running demos and cleanup.
 *  -  1 : Mixer initialization failed.
 *  - -1 : SDL video initialization failed.
 *  - -2 : SDL window creation failed.
 */
int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
        return -1;

    auto pWin = SDL_CreateWindow("for Keyboard Input...", 0, 0, 320, 200, 0);
    if (!pWin)
    {
        SDL_Quit();
        return -2;
    }

    auto mixer = audio::make_mixer<audio::sdl2::Mixer>(8, 44100, 1024);
    if (!mixer->init())
    {
        spdlog::error("can't init mixer");
        SDL_DestroyWindow(pWin);
        SDL_Quit();
        return 1;
    }

    const std::map<OplEmulator, std::string> emus = {
        {OplEmulator::DOS_BOX, "DOS_BOX"},
        //{ OplEmulator::MAME, "MAME" },
        //{ OplEmulator::NUKED, "NUKED" },
        //{ OplEmulator::WOODY, "WOODY" },
    };

    const std::map<OplType, std::string> types = {
        {OplType::OPL2, "OPL2"},
        //{OplType::DUAL_OPL2, "DUAL_OPL2"},
        //{OplType::OPL3, "OPL3"},
    };

    const std::string m = "##### {} {} #####";

    spdlog::set_level(spdlog::level::info);
    HyperSonicDrivers::utils::ILogger::instance->setLevelAll(HyperSonicDrivers::utils::ILogger::eLevel::Info);
    for (const auto &emu : emus)
    {
        for (const auto &type : types)
        {
            using enum fmt::color;

            for (const auto &c : {white_smoke, yellow, aqua,
                                  lime_green, blue_violet, indian_red})
            {
                spdlog::info(fmt::format(fg(c), fmt::runtime(m), emu.second, type.second));
            }

            try
            {
                adl_play(emu.first, type.first, mixer, "DUNE0.ADL");
                // adl_test(emu.first, type.first, mixer, "EOBSOUND.ADL", 1);
                // adl_test(emu.first, type.first, mixer, "LOREINTR.ADL", 3);
            }
            catch (const std::exception &e)
            {
                spdlog::default_logger()->error(e.what());
            }
        }
    }

    SDL_DestroyWindow(pWin);
    SDL_Quit();
    return 0;
}
