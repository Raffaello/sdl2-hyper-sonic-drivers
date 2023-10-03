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
        AdLibVoice* next = nullptr;
        AdLibVoice* prev = nullptr;

        uint8_t slot = 0; // NOTE: this is between 0 and 9, this is the "slot" (OPL channel number, not the MIDI Channel)
        uint8_t twoChan = 0;
        uint8_t vol1 = 0; // mod volume
        uint8_t vol2 = 0; // car volume
        int16_t duration = 0;

        Struct10 _s10a = { 0 };
        Struct11 _s11a = { 0 };
        Struct10 _s10b = { 0 };
        Struct11 _s11b = { 0 };

        // related to OPL3
        uint8_t secTwoChan = 0;
        uint8_t secVol1 = 0;
        uint8_t secVol2 = 0;

        AdLibVoice() {}
        inline void setNote(const uint8_t note) { m_note = note; }
        inline void setFree(const bool free) { m_free = free; };
        inline void setChannel(AdLibPart* chan) { m_channel = chan; };
        inline void setWaitForPedal(const bool waitForPedal) { m_sustain = waitForPedal; };
    };
}   
