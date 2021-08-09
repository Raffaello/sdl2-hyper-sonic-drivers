#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <utils/endianness.hpp>

namespace utils
{
    TEST(Endianness, swapLE16)
    {
        EXPECT_EQ(swapLE16(0x00A0), 0x00A0);
    }

    TEST(Endianness, swapBE16)
    {
        EXPECT_EQ(swapBE16(0x00A0), static_cast<int16_t>(0xA000));
    }

    TEST(Endianness, swapLE32)
    {
        EXPECT_EQ(swapLE32(0x000000A0), 0x000000A0);
    }

    TEST(Endianness, swapBE32)
    {
        EXPECT_EQ(swapBE32(0x000000A0), 0xA0000000);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
