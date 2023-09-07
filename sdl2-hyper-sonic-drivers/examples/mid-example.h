#pragma once

#include <HyperSonicDrivers/audio/sdl2/Mixer.hpp>

#include <HyperSonicDrivers/drivers/MIDDriver.hpp>
#include <HyperSonicDrivers/drivers/midi/devices/Native.hpp>
#include <HyperSonicDrivers/drivers/midi/devices/ScummVM.hpp>
#include <HyperSonicDrivers/drivers/midi/devices/Adlib.hpp>
#include <HyperSonicDrivers/drivers/midi/devices/SbPro2.hpp>
#include <HyperSonicDrivers/drivers/midi/Device.hpp>

#include <HyperSonicDrivers/files/dmx/OP2File.hpp>

#include <HyperSonicDrivers/hardware/opl/OPLFactory.hpp>
#include <HyperSonicDrivers/utils/algorithms.hpp>

#include <memory>
#include <spdlog/spdlog.h>
#include <fmt/chrono.h>
#include <fmt/color.h>
#include <map>

using namespace HyperSonicDrivers;

using hardware::opl::OPLFactory;
using hardware::opl::OplEmulator;
using hardware::opl::OplType;

void mid_test_run(drivers::MIDDriver& midDrv, const std::shared_ptr<audio::MIDI>& midi)
{
    auto start_time = std::chrono::system_clock::now();
    midDrv.play(midi);
    while (midDrv.isPlaying()) {
        utils::delayMillis(1000);
    }

    auto end_time = std::chrono::system_clock::now();
    auto tot_time = end_time - start_time;
    spdlog::info("Total Running Time: {:%M:%S}", tot_time);
}

void scummvm_mid_test(const OplEmulator emu, const OplType type, const std::shared_ptr<audio::IMixer>& mixer,
    const std::shared_ptr<audio::MIDI> midi)
{
    auto opl = OPLFactory::create(emu, type, mixer);
    if (opl == nullptr)
        return;

    const bool isOpl3 = type == OplType::OPL3;
    auto midi_device = std::make_shared<drivers::midi::devices::ScummVM>(opl, isOpl3);
    drivers::MIDDriver midDrv(/*mixer,*/ midi_device);

    spdlog::info("playing midi OPL3={}...", isOpl3);
    mid_test_run(midDrv, midi);
}

void mid_test(const OplEmulator emu, const OplType type, const std::shared_ptr<audio::IMixer>& mixer,
    const std::shared_ptr<audio::MIDI> midi)
{
    auto opl = OPLFactory::create(emu, type, mixer);
    if (opl == nullptr)
        return;

    const bool isOpl3 = type == OplType::OPL3;
    auto op2file = files::dmx::OP2File("GENMIDI.OP2");
    std::shared_ptr<drivers::midi::Device> midi_device;
    if (isOpl3)
        midi_device = std::make_shared<drivers::midi::devices::SbPro2>(opl, op2file.getBank());
    else
        midi_device = std::make_shared<drivers::midi::devices::Adlib>(opl, op2file.getBank());

    drivers::MIDDriver midDrv(/*mixer,*/ midi_device);

    spdlog::info("playing midi OPL3={}...", isOpl3);
    mid_test_run(midDrv, midi);
}

void mid_test_native(/*const std::shared_ptr<audio::IMixer>& mixer,*/
    const std::shared_ptr<audio::MIDI>& midi)
{
    auto nativeMidi = std::make_shared<drivers::midi::devices::Native>();

    drivers::MIDDriver mid_drv(/*mixer,*/ nativeMidi);

    spdlog::info("playing midi...");
    mid_test_run(mid_drv, midi);
}

int run(const std::shared_ptr<audio::MIDI>& midi, const bool use_opldrv)
{
    auto mixer = audio::make_mixer<audio::sdl2::Mixer>(8, 44100, 1024);
    if (!mixer->init())
    {
        std::cerr << "can't init the mixer" << std::endl;
        return 1;
    }

    // Reproducing MIDI file
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

    // Emulators
    using enum fmt::color;

    auto colors = {
        white_smoke, yellow,      aqua,
        lime_green,  blue_violet, indian_red };

    for (const auto& emu : emus)
    {
        for (const auto& type : types)
        {
            for (const auto& c : colors) {
                spdlog::info(fmt::format(fg(c), m, emu.second, type.second));
            }
            if (use_opldrv)
                mid_test(emu.first, type.first, mixer, midi);
            else
                scummvm_mid_test(emu.first, type.first, mixer, midi);
        }
    }

    // Native Midi
    for (auto& c : colors) {
        spdlog::info(fmt::format(fg(c), m, "Native", "MIDI"));
    }
    mid_test_native(/*mixer,*/ midi);

    return 0;
}
