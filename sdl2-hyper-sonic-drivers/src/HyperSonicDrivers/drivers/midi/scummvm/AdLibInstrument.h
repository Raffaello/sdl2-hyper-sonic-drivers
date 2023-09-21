#pragma once

#include <cstdint>

namespace HyperSonicDrivers::drivers::midi::scummvm
{
#pragma pack(push)
#pragma pack(1)
    struct InstrumentExtra
    {
        uint8_t a;
        uint8_t b;
        uint8_t c;
        uint8_t d;
        uint8_t e;
        uint8_t f;
        uint8_t g;
        uint8_t h;
    };

    struct AdLibInstrument
    {
        uint8_t modCharacteristic;      // trem_vibr_1
        uint8_t modScalingOutputLevel;  // scale_1
        uint8_t modAttackDecay;         // att_dec_1
        uint8_t modSustainRelease;      // sust_rel_1
        uint8_t modWaveformSelect;      // wave_1
        uint8_t carCharacteristic;      // trem_vibr_2
        uint8_t carScalingOutputLevel;  // scale_2
        uint8_t carAttackDecay;         // att_dec_2
        uint8_t carSustainRelease;      // sust_rel_2
        uint8_t carWaveformSelect;      // wave_2
        uint8_t feedback;               // feedback
        uint8_t flagsA;                 // unused?
        InstrumentExtra extraA;
        uint8_t flagsB;
        InstrumentExtra extraB;
        uint8_t duration;
    };
#pragma pack(pop)

    extern const AdLibInstrument g_gmInstruments[128];
    extern const AdLibInstrument g_gmPercussionInstruments[39];
    extern const AdLibInstrument g_gmInstrumentsOPL3[128][2];
    extern const AdLibInstrument g_gmPercussionInstrumentsOPL3[39][2];
    extern const uint8_t g_gmPercussionInstrumentMap[128];
    extern const uint8_t g_volumeTable[];
    extern uint8_t g_volumeLookupTable[64][32];

    extern int lookupVolume(int a, int b);
    extern void createLookupTable();
}
