#include <HyperSonicDrivers/drivers/midi/scummvm/MidiDriver.hpp>
#include <HyperSonicDrivers/utils/algorithms.hpp>

namespace HyperSonicDrivers::drivers::midi::scummvm
{
    const uint8_t MidiDriver::_mt32ToGm[128] = {
        //    0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
              0,   1,   0,   2,   4,   4,   5,   3,  16,  17,  18,  16,  16,  19,  20,  21, // 0x
              6,   6,   6,   7,   7,   7,   8, 112,  62,  62,  63,  63,  38,  38,  39,  39, // 1x
             88,  95,  52,  98,  97,  99,  14,  54, 102,  96,  53, 102,  81, 100,  14,  80, // 2x
             48,  48,  49,  45,  41,  40,  42,  42,  43,  46,  45,  24,  25,  28,  27, 104, // 3x
             32,  32,  34,  33,  36,  37,  35,  35,  79,  73,  72,  72,  74,  75,  64,  65, // 4x
             66,  67,  71,  71,  68,  69,  70,  22,  56,  59,  57,  57,  60,  60,  58,  61, // 5x
             61,  11,  11,  98,  14,   9,  14,  13,  12, 107, 107,  77,  78,  78,  76,  76, // 6x
             47, 117, 127, 118, 118, 116, 115, 119, 115, 112,  55, 124, 123,   0,  14, 117  // 7x
    };

    const uint8_t MidiDriver::_gmToMt32[128] = {
        //    0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
              5,   1,   2,   7,   3,   5,  16,  21,  22, 101, 101,  97, 104, 103, 102,  20, // 0x
              8,   9,  11,  12,  14,  15,  87,  15,  59,  60,  61,  62,  67,  44,  79,  23, // 1x
             64,  67,  66,  70,  68,  69,  28,  31,  52,  54,  55,  56,  49,  51,  57, 112, // 2x
             48,  50,  45,  26,  34,  35,  45, 122,  89,  90,  94,  81,  92,  95,  24,  25, // 3x
             80,  78,  79,  78,  84,  85,  86,  82,  74,  72,  76,  77, 110, 107, 108,  76, // 4x
             47,  44, 111,  45,  44,  34,  44,  30,  32,  33,  88,  34,  35,  35,  38,  33, // 5x
             41,  36, 100,  37,  40,  34,  43,  40,  63,  21,  99, 105, 103,  86,  55,  84, // 6x
            101, 103, 100, 120, 117, 113,  99, 128, 128, 128, 128, 124, 123, 128, 128, 128, // 7x
    };

    void MidiDriver::sendMT32Reset() {
        static const uint8_t resetSysEx[] = { 0x41, 0x10, 0x16, 0x12, 0x7F, 0x00, 0x00, 0x01, 0x00 };
        sysEx(resetSysEx, sizeof(resetSysEx));
        utils::delayMillis(100);
    }

    void MidiDriver::sendGMReset() {
        static const uint8_t gmResetSysEx[] = { 0x7E, 0x7F, 0x09, 0x01 };
        sysEx(gmResetSysEx, sizeof(gmResetSysEx));
        utils::delayMillis(100);

        // Send a Roland GS reset. This will be ignored by pure GM units,
        // but will enable certain GS features on units that support them.
        // This is especially useful for some Yamaha units, which are put
        // in XG mode after a GM reset, which has some compatibility
        // problems with GS features like instrument banks and
        // GS-exclusive drum sounds.
        static const uint8_t gsResetSysEx[] = { 0x41, 0x10, 0x42, 0x12, 0x40, 0x00, 0x7F, 0x00, 0x41 };
        sysEx(gsResetSysEx, sizeof(gsResetSysEx));
        utils::delayMillis(100);
    }
}
