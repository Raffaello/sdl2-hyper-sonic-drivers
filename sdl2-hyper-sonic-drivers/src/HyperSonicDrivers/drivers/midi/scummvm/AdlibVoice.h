#pragma once

#include <cstdint>
#include <cstring>
#include <HyperSonicDrivers/drivers/midi/scummvm/AdLibPart.hpp>
#include <HyperSonicDrivers/drivers/midi/IMidiChannelVoice.hpp>

namespace HyperSonicDrivers::drivers::midi::scummvm
{
    struct Struct10
    {
        uint8_t active;
        int16_t curVal;
        int16_t count;
        uint16_t maxValue;
        int16_t startValue;
        uint8_t loop;
        uint8_t tableA[4];
        uint8_t tableB[4];
        int8_t unk3;
        int8_t modWheel;
        int8_t modWheelLast;
        uint16_t speedLoMax;
        uint16_t numSteps;
        int16_t speedHi;
        int8_t direction;
        uint16_t speedLo;
        uint16_t speedLoCounter;
    };

    struct Struct11
    {
        int16_t modifyVal;
        uint8_t param;
        uint8_t flag0x40;
        uint8_t flag0x10;
        Struct10* s10;
    };

    class AdLibVoice : public IMidiChannelVoice
    {
    public:
        AdLibVoice* next;
        AdLibVoice* prev;

        uint8_t waitForPedal;
        uint8_t slot; // NOTE: this is between 0 and 9, this is the "slot" (OPL channel number, not the MIDI Channel)
        uint8_t twoChan;
        uint8_t vol1;
        uint8_t vol2;
        int16_t duration;

        Struct10 _s10a;
        Struct11 _s11a;
        Struct10 _s10b;
        Struct11 _s11b;

        // related to OPL3
        uint8_t secTwoChan;
        uint8_t secVol1;
        uint8_t secVol2;

        AdLibVoice() { memset(this, 0, sizeof(AdLibVoice)); }
        inline void setNote(const uint8_t note) { m_note = note; }
        inline void setFree(const bool free) { m_free = free; };
        inline void setChannel(AdLibPart* chan) { m_channel = chan; };
    };
}

