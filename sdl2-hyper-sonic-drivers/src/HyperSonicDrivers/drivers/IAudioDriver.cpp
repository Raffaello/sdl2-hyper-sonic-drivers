#include <HyperSonicDrivers/drivers/IAudioDriver.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>

namespace HyperSonicDrivers::drivers
{
    IAudioDriver::IAudioDriver(const std::shared_ptr<devices::IDevice>& device) :
        m_device(device)
    {
        if (m_device == nullptr)
        {
            utils::throwLogC<std::runtime_error>("device is null");
        }
    }
}
