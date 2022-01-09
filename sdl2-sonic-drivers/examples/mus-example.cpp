#include "mid-example.h"
#include <files/dmx/MUSFile.hpp>

int main(int argc, char* argv[])
{
    // Reproducing MIDI file
    auto musFile = std::make_shared<files::dmx::MUSFile>("D_E1M1.MUS");
    auto midi = musFile->getMIDI();

    return run(midi);
}
