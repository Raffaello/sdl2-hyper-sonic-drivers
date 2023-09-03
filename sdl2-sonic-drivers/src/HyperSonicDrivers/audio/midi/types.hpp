#pragma once

#include <vector>
#include <cstdint>
#include <cstddef>

namespace HyperSonicDrivers::audio::midi
{
    constexpr uint8_t MIDI_MAX_CHANNELS = 16;
    constexpr uint8_t MIDI_PERCUSSION_CHANNEL = 9; // standard MIDI percussion channel

    typedef std::vector<uint8_t> midi_vector_t;

    enum class MIDI_FORMAT
    {
        SINGLE_TRACK = 0,
        SIMULTANEOUS_TRACK = 1,
        MULTI_TRACK = 2
        //TODO: add XMI_FORMAT?
    };

    // move to MIDIEvent class ?
    typedef union MIDI_EVENT_type_u
    {
        uint8_t val;
        struct {
            uint8_t low : 4;
            uint8_t high : 4;
        };
    } MIDI_EVENT_type_u;

    enum class MIDI_META_EVENT_TYPES_LOW : uint8_t
    {
        SYS_EX0 = 0x00, // ???
        SYS_EX7 = 0x07, // ???
        META = 0xF   // MIDI META Event
    };

    enum class MIDI_META_EVENT : uint8_t
    {
        SEQUENCE_NUMBER = 0x00,
        TEXT = 0x01,
        COPYRIGHT = 0x02,
        SEQUENCE_NAME = 0x03,
        INSTRUMENT_NAME = 0x04,
        LYRICS = 0x05,
        MARKER = 0x06,
        CUE_POINT = 0x07,
        PROGRAM_NAME = 0x08,
        DEVICE_NAME = 0x09,
        CHANNEL_PREFIX = 0x20,
        MIDI_PORT = 0x21,
        END_OF_TRACK = 0x2F,
        SET_TEMPO = 0x51, // in microseconds per MIDI quarter note
        SMPTE_OFFSET = 0x54,
        TIME_SIGNATURE = 0x58,
        KEY_SIGNATURE = 0x59,
        SEQUENCER_SPECIFIC = 0x7F
    };

    // MIDI event types high values (Channel Voice Messages types)
    enum class MIDI_EVENT_TYPES_HIGH : uint8_t
    {
        NOTE_OFF = 0x8,
        NOTE_ON = 0x9,
        AFTERTOUCH = 0xA,
        CONTROLLER = 0xB,
        PROGRAM_CHANGE = 0xC,
        CHANNEL_AFTERTOUCH = 0xD,
        PITCH_BEND = 0xE,
        META_SYSEX = 0xF
    };

    enum class MIDI_META_EVENT_VAL : uint8_t
    {
        META = static_cast<uint8_t>((std::byte(MIDI_EVENT_TYPES_HIGH::META_SYSEX) << 4) | std::byte(MIDI_META_EVENT_TYPES_LOW::META)),
        SYS_EX0 = static_cast<uint8_t>((std::byte(MIDI_EVENT_TYPES_HIGH::META_SYSEX) << 4) | std::byte(MIDI_META_EVENT_TYPES_LOW::SYS_EX0)),
        SYS_EX7 = static_cast<uint8_t>((std::byte(MIDI_EVENT_TYPES_HIGH::META_SYSEX) << 4) | std::byte(MIDI_META_EVENT_TYPES_LOW::SYS_EX7)),
    };

    /*
    // TODO: too many repetitions to be an enum
    enum class MIDI_EVENT_CONTROLLER_TYPES : uint8_t
    {
        BANK_SELECT = 0,
        MODULATION_WHEEL = 1,
        BREATH_CONTROL = 2,
        FOOT_CONTROLLER = 4,
        PORTAMENTO_TIME = 5,
        DATA_ENTRY = 6,
        CHANNEL_VOLUME = 7, // (MAIN VOLUME)
        BALANCE = 8,         // (PAN)
        PAN = 10,
        EXPRESSION_CONTROLLER = 11,
        EFFECT_CONTROL_1 = 12,
        EFFECT_CONTROL_2 = 13,
        GENERAL_PURPOSE_CONTROLLER_1 = 16,
        GENERAL_PURPOSE_CONTROLLER_2 = 17,
        GENERAL_PURPOSE_CONTROLLER_3 = 18,
        GENERAL_PURPOSE_CONTROLLER_4 = 19,
        BANK_SELECT_2 = 32,
        MODULATION_WHEEL_2 = 33,
        BREATH_CONTROL_2 = 34,

    };
    */


    constexpr MIDI_EVENT_TYPES_HIGH TO_HIGH(const uint8_t x) { return static_cast<MIDI_EVENT_TYPES_HIGH>(x); }
    constexpr MIDI_META_EVENT_TYPES_LOW TO_META_LOW(const uint8_t x) { return static_cast<MIDI_META_EVENT_TYPES_LOW>(x); }
    constexpr MIDI_META_EVENT TO_META(const uint8_t x) { return static_cast<MIDI_META_EVENT>(x); }
}
