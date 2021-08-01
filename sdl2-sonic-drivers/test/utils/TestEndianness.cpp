#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <utils/endianness.hpp>

namespace utils
{
    TEST(Endianness, swap16LE)
    {
        EXPECT_EQ(swap16LE(0x00A0), 0x00A0);
    }

    TEST(Endianness, swap16BE)
    {
        EXPECT_EQ(swap16BE(0x00A0), static_cast<int16_t>(0xA000));
    }

    TEST(Endianness, swap32LE)
    {
        EXPECT_EQ(swap32LE(0x000000A0), 0x000000A0);
    }

    TEST(Endianness, swap32BE)
    {
        EXPECT_EQ(swap32BE(0x000000A0), 0xA0000000);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
