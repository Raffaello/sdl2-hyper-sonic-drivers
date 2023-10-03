#include <HyperSonicDrivers/drivers/midi/scummvm/MidiDriver_BASE.hpp>

namespace HyperSonicDrivers::drivers::midi::scummvm
{
    /*void MidiDriver_BASE::send(uint8_t status, uint8_t firstOp, uint8_t secondOp) {
        send(status | ((uint32_t)firstOp << 8) | ((uint32_t)secondOp << 16));
    }

    void MidiDriver_BASE::send(int8_t source, uint8_t status, uint8_t firstOp, uint8_t secondOp) {
        send(source, status | ((uint32_t)firstOp << 8) | ((uint32_t)secondOp << 16));
    }*/

    /*void MidiDriver_BASE::stopAllNotes(bool stopSustainedNotes)
    {
        using enum audio::midi::MIDI_EVENT_TYPES_HIGH;
        using enum audio::midi::MIDI_EVENT_CONTROLLER_TYPES;

        for (uint8_t i = 0; i < audio::midi::MIDI_MAX_CHANNELS; ++i)
        {
            controller(i, ALL_NOTES_OFF, 0);
            if (stopSustainedNotes)
                controller(i, SUSTAIN, 0);
        }
    }*/
}
