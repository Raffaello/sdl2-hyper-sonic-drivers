#pragma once

#include <drivers/midi/scummvm/AdLibPart.hpp>
#include <cstdint>
#include <cstring>

namespace drivers
{
    namespace midi
    {
        namespace scummvm
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

            struct AdLibVoice
            {
                AdLibPart* _part;
                AdLibVoice* _next, * _prev;
                uint8_t _waitForPedal;
                uint8_t _note;
                uint8_t _channel;
                uint8_t _twoChan;
                uint8_t _vol1, _vol2;
                int16_t _duration;

                Struct10 _s10a;
                Struct11 _s11a;
                Struct10 _s10b;
                Struct11 _s11b;

                uint8_t _secTwoChan;
                uint8_t _secVol1, _secVol2;

                AdLibVoice() { memset(this, 0, sizeof(AdLibVoice)); }
            };
        }
    }
}

