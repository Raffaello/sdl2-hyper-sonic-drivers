#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <HyperSonicDrivers/drivers/IMusicDriver.hpp>

namespace HyperSonicDrivers::drivers
{
    class IMusicDriverMock : public IMusicDriver
    {
    public:
        IMusicDriverMock(const std::shared_ptr<devices::IDevice>& device) : IMusicDriver(device) {}
        void play(const uint8_t track) noexcept override {};
        void stop() noexcept override {};
        bool isPlaying() const noexcept override { return false; };
    };

    TEST(IMusicDriver, cstor_nullptr)
    {
        EXPECT_THROW(IMusicDriverMock md(nullptr), std::runtime_error);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}