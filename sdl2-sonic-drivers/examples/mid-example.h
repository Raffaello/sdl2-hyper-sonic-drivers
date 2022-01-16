#pragma once

#include <audio/scummvm/Mixer.hpp>
#include <audio/scummvm/SDLMixerManager.hpp>
#include <drivers/MIDDriver.hpp>
#include <drivers/midi/devices/Native.hpp>
#include <drivers/midi/devices/ScummVM.hpp>

#include <hardware/opl/scummvm/Config.hpp>
#include <utils/algorithms.hpp>

#include <memory>
#include <spdlog/spdlog.h>
#include <fmt/chrono.h>
#include <fmt/color.h>
#include <map>

using hardware::opl::scummvm::Config;
using hardware::opl::scummvm::OplEmulator;

void mid_test(const OplEmulator emu, const Config::OplType type, std::shared_ptr<audio::scummvm::Mixer> mixer, const std::shared_ptr<audio::MIDI> midi)
{
    auto opl = Config::create(emu, type, mixer);
    if (opl == nullptr)
        return;

    const bool isOpl3 = type == Config::OplType::OPL3;
    auto scumm_midi_device = std::make_shared<drivers::midi::devices::ScummVM>(opl, isOpl3);
    drivers::MIDDriver midDrv(mixer, scumm_midi_device);

    spdlog::info("playing midi OPL3={}...", isOpl3);
    auto start_time = std::chrono::system_clock::now();
    midDrv.play(midi);
    while (midDrv.isPlaying()) {
        utils::delayMillis(1000);
    }
    auto end_time = std::chrono::system_clock::now();
    auto tot_time = end_time - start_time;
    spdlog::info("Total Running Time: {:%M:%S}", tot_time);
}

void mid_test_native(std::shared_ptr<audio::scummvm::Mixer> mixer, const std::shared_ptr<audio::MIDI> midi)
{
    auto nativeMidi = std::make_shared<drivers::midi::devices::Native>();

    drivers::MIDDriver mid_drv(mixer, nativeMidi);

    spdlog::info("playing midi...");
    auto start_time = std::chrono::system_clock::now();
    mid_drv.play(midi);
    auto end_time = std::chrono::system_clock::now();
    auto tot_time = end_time - start_time;
    spdlog::info("Total Running Time: {:%M:%S}", tot_time);
}

int run(const std::shared_ptr<audio::MIDI> midi)
{
    audio::scummvm::SdlMixerManager mixerManager;
    mixerManager.init();

    auto mixer = mixerManager.getMixer();

    // Reproducing MIDI file
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

    // Emulators
    for (const auto& emu : emus)
    {
        for (const auto& type : types)
        {
            for (const auto& c : { fmt::color::white_smoke, fmt::color::yellow,      fmt::color::aqua,
                             fmt::color::lime_green,  fmt::color::blue_violet, fmt::color::indian_red }) {
                spdlog::info(fmt::format(fg(c), m, emu.second, type.second));
            }

            mid_test(emu.first, type.first, mixer, midi);
        }
    }

    // Native Midi
    for (auto& c : { fmt::color::white_smoke, fmt::color::yellow,      fmt::color::aqua,
                             fmt::color::lime_green,  fmt::color::blue_violet, fmt::color::indian_red }) {
        spdlog::info(fmt::format(fg(c), m, "Native", "MIDI"));
    }
    mid_test_native(mixer, midi);

    return 0;
}
