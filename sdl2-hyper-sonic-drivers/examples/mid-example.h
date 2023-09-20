#pragma once

#include <HyperSonicDrivers/audio/sdl2/Mixer.hpp>

#include <HyperSonicDrivers/drivers/MIDDriver.hpp>
#include <HyperSonicDrivers/devices/midi/MidiNative.hpp>
#include <HyperSonicDrivers/devices/midi/MidiScummVM.hpp>
#include <HyperSonicDrivers/devices/midi/MidiAdlib.hpp>
#include <HyperSonicDrivers/devices/midi/MidiSbPro.hpp>
#include <HyperSonicDrivers/devices/midi/MidiSbPro2.hpp>
#include <HyperSonicDrivers/devices/IMidiDevice.hpp>
#include <HyperSonicDrivers/devices/makers.hpp>

#include <HyperSonicDrivers/files/dmx/OP2File.hpp>

#include <HyperSonicDrivers/hardware/opl/OPLFactory.hpp>
#include <HyperSonicDrivers/utils/algorithms.hpp>
#include <std/OplTypeFormatter.hpp>

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
    auto midi_device = std::make_shared<devices::midi::MidiScummVM>(opl, isOpl3, audio::mixer::eChannelGroup::Music);
    drivers::MIDDriver midDrv(/*mixer,*/ midi_device);

    spdlog::info("playing midi OPL3={}...", isOpl3);
    mid_test_run(midDrv, midi);
}

void mid_test(const OplEmulator emu, const OplType type, const std::shared_ptr<audio::IMixer>& mixer,
    const std::shared_ptr<audio::MIDI> midi)
{
    auto op2file = files::dmx::OP2File("GENMIDI.OP2");
    std::shared_ptr<devices::IMidiDevice> midi_device;
    switch (type)
    {
        using enum OplType;
        using namespace devices;

    case OPL2:
        midi_device = make_midi_device<midi::MidiAdlib>(mixer, op2file.getBank(), audio::mixer::eChannelGroup::Music, emu);
        break;
    case DUAL_OPL2:
        midi_device = make_midi_device<midi::MidiSbPro>(mixer, op2file.getBank(), audio::mixer::eChannelGroup::Music, emu);
        break;
    case OPL3:
        midi_device = make_midi_device<midi::MidiSbPro2>(mixer, op2file.getBank(), audio::mixer::eChannelGroup::Music, emu);
        break;
    default:
        throw std::runtime_error("?");
    }

    drivers::MIDDriver midDrv(/*mixer,*/ midi_device);

    spdlog::info(std::format("playing midi (OPL type={})...", type));
    mid_test_run(midDrv, midi);
}

void mid_test_native(/*const std::shared_ptr<audio::IMixer>& mixer,*/
    const std::shared_ptr<audio::MIDI>& midi)
{
    auto nativeMidi = std::make_shared<devices::midi::MidiNative>();

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

    // Emulators
    using enum fmt::color;

    const auto colors = {
        white_smoke, yellow,      aqua,
        lime_green,  blue_violet, indian_red };

    for (const auto& emu : emus)
    {
        for (const auto& type : types)
        {
            try
            {
                for (const auto& c : colors)
                    spdlog::info(fmt::format(fg(c), m, emu.second, type.second));

                if (use_opldrv)
                    mid_test(emu.first, type.first, mixer, midi);
                else
                    scummvm_mid_test(emu.first, type.first, mixer, midi);
            }
            catch (const std::exception& e)
            {
                spdlog::default_logger()->error(e.what());
            }
        }
    }

    // Native Midi
    for (const auto& c : colors) {
        spdlog::info(fmt::format(fg(c), m, "Native", "MIDI"));
    }
    mid_test_native(/*mixer,*/ midi);

    return 0;
}
