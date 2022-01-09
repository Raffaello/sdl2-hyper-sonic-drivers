#include "mid-example.h"
#include <files/MIDFile.hpp>

int main(int argc, char* argv[])
{
    auto midFile = std::make_shared<files::MIDFile>("MI_intro.mid");
    auto midi = midFile->convertToSingleTrackMIDI();
    return run(midi);
}
