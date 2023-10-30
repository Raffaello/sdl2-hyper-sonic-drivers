#include <HyperSonicDrivers/drivers/IMusicDriver.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>

namespace HyperSonicDrivers::drivers
{
    IMusicDriver::IMusicDriver(const std::shared_ptr<devices::IDevice>& device) :
        m_device(device)
    {
        if (m_device == nullptr)
        {
            utils::throwLogC<std::runtime_error>("device is null");
        }
    }
}
