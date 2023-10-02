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
    };

    typedef union MIDI_EVENT_type_u
    {
        uint8_t val;
        struct {
            uint8_t low : 4;
            uint8_t high : 4;
        };
    } MIDI_EVENT_type_u;
    static_assert(sizeof(uint8_t) == sizeof(MIDI_EVENT_type_u));

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

    enum class MIDI_EVENT_CONTROLLER_TYPES : uint8_t
    {
        BANK_SELECT_MSB = 0,
        MODULATION_WHEEL = 1,
        BREATH_CONTROL = 2,
        FOOT_CONTROLLER = 4,
        PORTAMENTO_TIME = 5,
        DATA_ENTRY_MSB = 6,
        CHANNEL_VOLUME = 7,
        BALANCE = 8,
        PAN = 10,
        EXPRESSION_CONTROLLER = 11,
        EFFECT_CONTROL_1 = 12,
        EFFECT_CONTROL_2 = 13,
        GENERAL_PURPOSE_CONTROLLER_1 = 16,
        GENERAL_PURPOSE_CONTROLLER_2 = 17,
        GENERAL_PURPOSE_CONTROLLER_3 = 18,
        GENERAL_PURPOSE_CONTROLLER_4 = 19,
        BANK_SELECT_LSB = 32,
        MODULATION_WHEEL_2 = 33,
        BREATH_CONTROL_2 = 34,
        DATA_ENTRY_LSB = 38,
        SUSTAIN = 64,
        REVERB = 91,
        TREMOLO = 92,
        CHORUS = 93,
        DETUNE = 94,
        PHASER = 95,
        RPN_LSB = 100,
        RPN_MSB = 101,
        ALL_SOUND_OFF = 120,
        RESET_ALL_CONTROLLERS = 121,
        ALL_NOTES_OFF = 123,

        // eXtended MIDI
        CHANNEL_LOCK = 110,
        CHANNEL_LOCK_PROTECT = 111,
        VOICE_PROTECT = 112,
        TIMBRE_PROTECT = 113,
        PATCH_BANK_SELECT = 114,
        INDIRECT_CONTROLLER_PREFIX = 115,
        FOR_LOOP_CONTROLLER = 116,
        NEXT_BREAK_LOOP_CONTROLLER = 117,
        CLEAR_BEAT_BAR_COUNT = 118,
        CALLBACK_TRIGGER = 119,
        SEQUENCE_BRANCH_INDEX = 120,
    };

    constexpr MIDI_EVENT_TYPES_HIGH TO_HIGH(const uint8_t x) { return static_cast<MIDI_EVENT_TYPES_HIGH>(x); }
    constexpr MIDI_META_EVENT_TYPES_LOW TO_META_LOW(const uint8_t x) { return static_cast<MIDI_META_EVENT_TYPES_LOW>(x); }
    constexpr MIDI_META_EVENT TO_META(const uint8_t x) { return static_cast<MIDI_META_EVENT>(x); }
    constexpr MIDI_EVENT_CONTROLLER_TYPES TO_CTRL(const uint8_t x) { return static_cast<MIDI_EVENT_CONTROLLER_TYPES>(x); }
}
