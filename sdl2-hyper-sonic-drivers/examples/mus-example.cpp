#include "mid-example.h"
#include <HyperSonicDrivers/files/dmx/MUSFile.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>

using namespace HyperSonicDrivers;

int main(int argc, char* argv[])
{
    utils::ILogger::instance->setLevelAll(utils::ILogger::eLevel::Debug);
    // Reproducing MIDI file
    auto musFile = std::make_shared<files::dmx::MUSFile>("D_E1M1.MUS");
    auto midi = musFile->getMIDI();

    return run(midi, true);
}
