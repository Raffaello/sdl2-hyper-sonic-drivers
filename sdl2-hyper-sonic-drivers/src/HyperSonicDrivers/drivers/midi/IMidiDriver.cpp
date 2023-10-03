#include <HyperSonicDrivers/drivers/midi/IMidiDriver.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>
#include <format>

namespace HyperSonicDrivers::drivers::midi
{
    using audio::midi::TO_HIGH;
    using utils::logW;

    void IMidiDriver::send(const audio::midi::MIDIEvent& e) noexcept
    {
        // TODO: sysEx must be reviewed if ok, probably shoudl check
        //       also if last byte is meta sysEx end ...
        using audio::midi::TO_HIGH;
        using audio::midi::MIDI_EVENT_TYPES_HIGH;

        switch (TO_HIGH(e.type.high))
        {
        case MIDI_EVENT_TYPES_HIGH::META_SYSEX:
            sysEx(e.data.data(), static_cast<uint16_t>(e.data.size()));
            break;
        default:
            send(e.toUint32());
        }
    }

    void IMidiDriver::send(int8_t channel, uint32_t msg) noexcept
    {
        using audio::midi::MIDI_EVENT_type_u;
        using audio::midi::MIDI_EVENT_TYPES_HIGH;

        const uint8_t param2 = (uint8_t)((msg >> 16) & 0xFF);
        const uint8_t param1 = (uint8_t)((msg >> 8) & 0xFF);
        MIDI_EVENT_type_u cmd;
        cmd.high = static_cast<uint8_t>((msg >> 4) & 0xF);

        send(TO_HIGH(cmd.high), channel, param1, param2);
    }

    void IMidiDriver::send(uint32_t msg) noexcept
    {
        send(msg & 0xF, msg & 0xFFFFFFF0);
    }

    void IMidiDriver::send(const audio::midi::MIDI_EVENT_TYPES_HIGH type, const uint8_t channel, const uint8_t data1, const uint8_t data2)
    {
        using audio::midi::MIDI_EVENT_TYPES_HIGH;
        using audio::midi::TO_CTRL;

        switch (type)
        {
        case MIDI_EVENT_TYPES_HIGH::NOTE_OFF:// Note Off
            noteOff(channel, data1);
            break;
        case MIDI_EVENT_TYPES_HIGH::NOTE_ON: // Note On
            noteOn(channel, data1, data2);
            break;
        case MIDI_EVENT_TYPES_HIGH::AFTERTOUCH: // Aftertouch
            break; // Not supported.
        case MIDI_EVENT_TYPES_HIGH::CONTROLLER: // Control Change
            controller(channel, TO_CTRL(data1), data2);
            break;
        case MIDI_EVENT_TYPES_HIGH::PROGRAM_CHANGE: // Program Change
            programChange(channel, data1);
            break;
        case MIDI_EVENT_TYPES_HIGH::CHANNEL_AFTERTOUCH: // Channel Pressure
            break; // Not supported.
        case MIDI_EVENT_TYPES_HIGH::PITCH_BEND: // Pitch Bend
        {
            const auto bend = static_cast<uint16_t>((data1 | (data2 << 7)) - audio::midi::MIDI_PITCH_BEND_DEFAULT);
            pitchBend(channel, bend);
        }
        break;
        case MIDI_EVENT_TYPES_HIGH::META_SYSEX: // SysEx
            // We should never get here! SysEx information has to be
            // sent via high-level semantic methods.
            logW("Receiving SysEx command on a send() call");
            break;

        default:
            logW(std::format("Unknown send() command {:#0x}", static_cast<uint8_t>(type)));
        }
    }

    void IMidiDriver::controller(const uint8_t chan, const audio::midi::MIDI_EVENT_CONTROLLER_TYPES ctrl_type, uint8_t value) noexcept
    {
        // MIDI_EVENT_CONTROLLER_TYPES
        switch (ctrl_type)
        {
            using enum audio::midi::MIDI_EVENT_CONTROLLER_TYPES;
        case BANK_SELECT_MSB:
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
            ctrl_reverb(chan, value);
            break;
        case CHORUS:
            ctrl_chorus(chan, value);
            break;
        case RESET_ALL_CONTROLLERS:
            // reset all controllers
            logW("reset all controllers value not implemented");
            break;
        case ALL_NOTES_OFF:
            ctrl_allNotesOff();
            break;
        default:
            logW(std::format("OplDriver: Unknown control change message {:d} {:d}", static_cast<uint8_t>(ctrl_type), value));
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
