#include "mid-example.h"
#include <HyperSonicDrivers/files/MIDFile.hpp>

#include <SDL2/SDL_main.h>

using namespace HyperSonicDrivers;

int main(int argc, char* argv[])
{
    auto midFile = std::make_shared<files::MIDFile>("MI_intro.mid");
    auto midi = midFile->getMIDI();
    // use scummvm opl midi driver
    return run(midi, false);
}
