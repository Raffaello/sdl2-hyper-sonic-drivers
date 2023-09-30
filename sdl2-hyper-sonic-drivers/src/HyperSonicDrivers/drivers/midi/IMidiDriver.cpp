#include <HyperSonicDrivers/drivers/midi/IMidiDriver.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>

namespace HyperSonicDrivers::drivers::midi
{
    using audio::midi::TO_HIGH;
    using utils::logW;

    void IMidiDriver::send(const audio::midi::MIDIEvent& e) noexcept
    {
        send(e.toUint32());
    }

    void IMidiDriver::send(int8_t channel, uint32_t msg) noexcept
    {
        using audio::midi::MIDI_EVENT_type_u;
        using audio::midi::MIDI_EVENT_TYPES_HIGH;

        const uint8_t param2 = (uint8_t)((msg >> 16) & 0xFF);
        const uint8_t param1 = (uint8_t)((msg >> 8) & 0xFF);
        MIDI_EVENT_type_u cmd;
        cmd.high = static_cast<uint8_t>((msg >> 4) & 0xF);

        switch (TO_HIGH(cmd.high))
        {
        case MIDI_EVENT_TYPES_HIGH::NOTE_OFF:// Note Off
            noteOff(channel, param1);
            break;
        case MIDI_EVENT_TYPES_HIGH::NOTE_ON: // Note On
            noteOn(channel, param1, param2);
            break;
        case MIDI_EVENT_TYPES_HIGH::AFTERTOUCH: // Aftertouch
            break; // Not supported.
        case MIDI_EVENT_TYPES_HIGH::CONTROLLER: // Control Change
            controller(channel, param1, param2);
            break;
        case MIDI_EVENT_TYPES_HIGH::PROGRAM_CHANGE: // Program Change
            programChange(channel, param1);
            break;
        case MIDI_EVENT_TYPES_HIGH::CHANNEL_AFTERTOUCH: // Channel Pressure
            break; // Not supported.
        case MIDI_EVENT_TYPES_HIGH::PITCH_BEND: // Pitch Bend
            pitchBend(channel, static_cast<uint16_t>((param1 | (param2 << 7)) - 0x2000));
            break;
        case MIDI_EVENT_TYPES_HIGH::META_SYSEX: // SysEx
            // We should never get here! SysEx information has to be
            // sent via high-level semantic methods.
            logW("Receiving SysEx command on a send() call");
            break;

        default:
            logW(std::format("Unknown send() command {:#0x}", cmd.val));
        }
    }

    void IMidiDriver::send(uint32_t msg) noexcept
    {
        send(msg & 0xF, msg & 0xFFFFFFF0);
    }
}
