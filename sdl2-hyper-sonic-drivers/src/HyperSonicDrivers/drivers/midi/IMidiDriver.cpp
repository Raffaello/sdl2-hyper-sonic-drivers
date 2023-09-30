#include <HyperSonicDrivers/drivers/midi/IMidiDriver.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>
#include <format>

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
        {
            const auto bend = static_cast<uint16_t>((param1 | (param2 << 7)) - 0x2000);
            pitchBend(channel, bend);
        }
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

    void IMidiDriver::controller(const uint8_t chan, const uint8_t ctrl, uint8_t value) noexcept
    {
        using audio::midi::TO_CTRL;
        // MIDI_EVENT_CONTROLLER_TYPES
        switch (TO_CTRL(ctrl))
        {
            using enum audio::midi::MIDI_EVENT_CONTROLLER_TYPES;
        case BANK_SELECT:
        //case BANK_SELECT_2:
            // Bank select. Not supported
            logW(std::format("bank select value {}", value));
            break;
        case MODULATION_WHEEL:
            ctrl_modulationWheel(chan, value);
            break;
        case CHANNEL_VOLUME:
            ctrl_volume(chan, value);
            break;
        case PAN:
            ctrl_panPosition(chan, value);
            break;
        case GENERAL_PURPOSE_CONTROLLER_1:
            //pitchBendFactor(value);
            logW(std::format("pitchBendFactor value {}", value));
            break;
        case GENERAL_PURPOSE_CONTROLLER_2:
            //detune(value);
            logW(std::format("detune value {}", value));
            break;
        case GENERAL_PURPOSE_CONTROLLER_3:
            //priority(value);
            logW(std::format("priority value {}", value));
            break;
        case SUSTAIN:
            ctrl_sustain(chan, value);
            break;
        case REVERB:
            // Effects level. Not supported.
            //effectLevel(value);
            logW(std::format("effect level value {}", value));
            break;
        case CHORUS:
            // Chorus level. Not supported.
            //chorusLevel(value);
            logW(std::format("chorus level value {}", value));
            break;
        //case 119:
        //    // Unknown, used in Simon the Sorcerer 2
        //    logW(std::format("unknown value {}", value));
        //    break;
        case RESET_ALL_CONTROLLERS:
            // reset all controllers
            logW("reset all controllers value");
            //modulationWheel(0);
            //pitchBendFactor(0);
            //detune(0);
            //sustain(false);
            break;
        case ALL_NOTES_OFF:
            ctrl_allNotesOff();
            break;
        default:
            logW(std::format("OplDriver: Unknown control change message {:d} {:d}", ctrl, value));
        }
    }

    void IMidiDriver::programChange(const uint8_t chan, const uint8_t program) noexcept
    {
        if (program > 127)
        {
            logW(std::format("Progam change value >= 127 -> {}", program));
        }

        m_channels[chan]->program = program;
    }
}
