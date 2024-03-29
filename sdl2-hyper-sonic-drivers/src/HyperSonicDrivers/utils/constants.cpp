#include <HyperSonicDrivers/utils/constants.hpp>
#include <HyperSonicDrivers/version.h>

namespace HyperSonicDrivers::utils
{
    const char* getVersion()
    {
        return __HYPER_SONIC_DRIVERS_VERSION;
    }
    int getVersionMajor()
    {
        return __HYPER_SONIC_DRIVERS_VERSION_MAJOR;
    }
    int getVersionMinor()
    {
        return __HYPER_SONIC_DRIVERS_VERSION_MINOR;
    }
    int getVersionPatch()
    {
        return __HYPER_SONIC_DRIVERS_VERSION_PATCH;
    }
}
