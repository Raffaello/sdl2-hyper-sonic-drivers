#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <files/AILDriverFile.hpp>

namespace files
{
    TEST(AILDriverFile, ADLIB)
    {
        AILDriverFile drv("fixtures/ADLIB.ADV");
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
