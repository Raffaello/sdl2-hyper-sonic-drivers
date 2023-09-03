#include <drivers/midi/scummvm/MidiDriver_BASE.hpp>

namespace HyperSonicDrivers::drivers::midi::scummvm
{
    void MidiDriver_BASE::send(uint8_t status, uint8_t firstOp, uint8_t secondOp) {
        send(status | ((uint32_t)firstOp << 8) | ((uint32_t)secondOp << 16));
    }

    void MidiDriver_BASE::send(int8_t source, uint8_t status, uint8_t firstOp, uint8_t secondOp) {
        send(source, status | ((uint32_t)firstOp << 8) | ((uint32_t)secondOp << 16));
    }

    void MidiDriver_BASE::stopAllNotes(bool stopSustainedNotes) {
        for (int i = 0; i < 16; ++i) {
            send(0xB0 | i, MIDI_CONTROLLER_ALL_NOTES_OFF, 0);
            if (stopSustainedNotes)
                send(0xB0 | i, MIDI_CONTROLLER_SUSTAIN, 0); // Also send a sustain off event (bug #5524)
        }
    }
}
