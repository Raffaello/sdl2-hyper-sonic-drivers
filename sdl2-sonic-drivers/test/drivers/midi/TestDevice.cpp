#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <drivers/midi/Device.hpp>
#include <drivers/midi/devices/SpyDevice.hpp>
#include <drivers/MIDDriverMock.hpp>
#include <audio/stubs/StubMixer.hpp>

namespace drivers::midi
{
    TEST(Device, acquire_release)
    {
        using audio::stubs::StubMixer;
        
        auto mixer = std::make_shared<StubMixer>();
        auto device = std::make_shared<devices::SpyDevice>();

        MIDDriverMock middrv(mixer, device);
        MIDDriverMock middrv2(mixer, device);
        
        EXPECT_FALSE(device->isAcquired());
        EXPECT_TRUE(device->release(&middrv));
        EXPECT_TRUE(device->release(&middrv2));
        EXPECT_TRUE(device->release(nullptr));

        EXPECT_FALSE(device->isOwned(&middrv));
        EXPECT_FALSE(device->isOwned(&middrv2));
        EXPECT_TRUE(device->isOwned(nullptr));

        EXPECT_TRUE(device->acquire(&middrv));
        EXPECT_FALSE(device->acquire(&middrv2));
        EXPECT_FALSE(device->acquire(nullptr));

        EXPECT_TRUE(device->isAcquired());
        EXPECT_TRUE(device->isOwned(&middrv));

        EXPECT_FALSE(device->release(&middrv2));
        EXPECT_FALSE(device->release(nullptr));

        EXPECT_TRUE(device->release(&middrv));
        EXPECT_TRUE(device->release(&middrv2));
        EXPECT_TRUE(device->release(nullptr));

        EXPECT_FALSE(device->isAcquired());
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
