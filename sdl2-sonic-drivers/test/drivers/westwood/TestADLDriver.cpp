#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <drivers/westwood/ADLDriver.hpp>

namespace drivers
{
    namespace westwood
    {
        TEST(ADLDriver, init)
        {
        }
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
