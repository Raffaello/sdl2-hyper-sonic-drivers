#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <utils/endianness.hpp>

namespace utils
{
    TEST(Endianness, read16LE)
    {
        EXPECT_EQ(read16LE(0x00A0), 0x00A0);
    }

    TEST(Endianness, read16BE)
    {
        EXPECT_EQ(read16BE(0x00A0), static_cast<int16_t>(0xA000));
    }

    TEST(Endianness, readLE32)
    {
        EXPECT_EQ(readLE32(0x000000A0), 0x000000A0);
    }

    TEST(Endianness, read32BE)
    {
        EXPECT_EQ(read32BE(0x000000A0), 0xA0000000);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
