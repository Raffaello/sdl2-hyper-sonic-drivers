#pragma once

namespace HyperSonicDrivers::devices
{
enum class eDeviceType
{
    Opl = 0,
#if HAS_MT32_EMU
    Mt32,
#endif
};

enum class eDeviceName
{
    Adlib,
    SbPro,
    SbPro2,
#if HAS_MT32_EMU
    Mt32
#endif
};
}    // namespace HyperSonicDrivers::devices
