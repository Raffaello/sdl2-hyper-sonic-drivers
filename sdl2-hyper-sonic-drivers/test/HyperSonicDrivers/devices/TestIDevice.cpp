#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <HyperSonicDrivers/devices/IDevice.hpp>
#include <HyperSonicDrivers/devices/SpyDevice.hpp>
#include <HyperSonicDrivers/drivers/MIDDriverMock.hpp>

namespace HyperSonicDrivers::devices
{
    using drivers::MIDDriverMock;

    TEST(IDevice, double_acquire)
    {
        auto device = std::make_shared<SpyDevice>();

        MIDDriverMock middrv(device);
        EXPECT_THROW(MIDDriverMock middrv2(device), std::runtime_error);
    }

    TEST(IDevice, acquire_release)
    {
        auto device = std::make_shared<SpyDevice>();

        MIDDriverMock middrv(device);
        
        EXPECT_TRUE(device->isAcquired());
        EXPECT_TRUE(device->release(&middrv));
        EXPECT_TRUE(device->release(nullptr));

        EXPECT_FALSE(device->isOwned(&middrv));
        EXPECT_TRUE(device->isOwned(nullptr));

        EXPECT_TRUE(device->acquire(&middrv));
        EXPECT_FALSE(device->acquire(nullptr));
        EXPECT_TRUE(device->acquire(&middrv));

        EXPECT_TRUE(device->isAcquired());
        EXPECT_TRUE(device->isOwned(&middrv));

        EXPECT_FALSE(device->release(nullptr));

        EXPECT_TRUE(device->release(&middrv));
        EXPECT_TRUE(device->release(nullptr));

        EXPECT_FALSE(device->isAcquired());
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
