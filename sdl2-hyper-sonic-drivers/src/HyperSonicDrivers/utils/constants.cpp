#include <HyperSonicDrivers/utils/constants.hpp>
#include <HyperSonicDrivers/version.h>

namespace HyperSonicDrivers::utils
{
    const char* getVersion()
    {
        return __SONIC_DRIVERS_VERSION;
    }
    int getVersionMajor()
    {
        return __SONIC_DRIVERS_VERSION_MAJOR;
    }
    int getVersionMinor()
    {
        return __SONIC_DRIVERS_VERSION_MINOR;
    }
    int getVersionPatch()
    {
        return __SONIC_DRIVERS_VERSION_PATCH;
    }
}
