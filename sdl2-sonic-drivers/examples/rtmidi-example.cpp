#include <audio/scummvm/Mixer.hpp>
#include <audio/scummvm/SDLMixerManager.hpp>
#include <utils/algorithms.hpp>

#include <iostream>
#include <SDL2/SDL.h>

#include <RtMidi.h>
#include <memory>
#include <vector>

#include <files/MIDFile.hpp>
#include <drivers/MIDParser.hpp>
#include <spdlog/spdlog.h>


using namespace audio::scummvm;
using std::cout;
using std::endl;

int main(int argc, char* argv[])
{
    SdlMixerManager mixerManager;
    mixerManager.init();

    auto mixer = mixerManager.getMixer();
    std::shared_ptr<RtMidiOut> midiout;

    try {
        midiout = std::make_shared<RtMidiOut>();
    }
    catch (RtMidiError& error) {
        // Handle the exception here
        error.printMessage();
    }

    // Check outputs.
    unsigned int nPorts = midiout->getPortCount();
    std::string portName;
    std::cout << "\nThere are " << nPorts << " MIDI output ports available.\n";
    for (unsigned int i = 0; i < nPorts; i++) {
        try {
            portName = midiout->getPortName(i);
        }
        catch (RtMidiError& error) {
            error.printMessage();
        }
        std::cout << "  Output Port #" << i + 1 << ": " << portName << endl;
    }
    std::cout << endl;

    cout << "Midi note test...";
    // Open first available port.
    midiout->openPort(0);
    // Send out a series of MIDI messages.
    std::vector<unsigned char> message;
    // Program change: 192, 5
    message.push_back(192);
    message.push_back(5);
    midiout->sendMessage(&message);
    // Control Change: 176, 7, 100 (volume)
    message[0] = 176;
    message[1] = 7;
    message.push_back(100);
    midiout->sendMessage(&message);
    // Note On: 144, 64, 90
    message[0] = 144;
    message[1] = 64;
    message[2] = 90;
    midiout->sendMessage(&message);
    utils::delayMillis(1000);
    // Note Off: 128, 64, 40
    message[0] = 128;
    message[1] = 64;
    message[2] = 40;
    midiout->sendMessage(&message);

    cout << "OK" << endl;

    // Reproducing MIDI file
    auto midFile = std::make_shared<files::MIDFile>("MI_intro.mid");
    auto midi = midFile->getMIDI();
    drivers::MIDParser midParser(midi, mixer);

    // quantizing to send messages to midiout from the midi file:
    // at the moment can be done within the parser in a "raw mode",
    // injecting the rtmidiout object; then creating an abstract
    // "device::midiOut" interface to send messages etc that will
    // be implemented for each hardware emulators.

    // also it would be nice to have a sort of "callback" functionality
    // to display in real time the notes (messages) being played.
    // can be done easily in the console like guitar hero
    // screen stream for the visualization.


    spdlog::set_level(spdlog::level::debug);
    midParser.display(midiout);

    return 0;
}
