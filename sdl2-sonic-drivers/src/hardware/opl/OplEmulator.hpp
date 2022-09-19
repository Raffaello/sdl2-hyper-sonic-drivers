#pragma once

namespace hardware::opl
{
    enum class OplEmulator
    {
        AUTO = 0,
        MAME = 1,
        DOS_BOX = 2,
        NUKED = 4,
        WOODY = 8
    };
}
