#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <HyperSonicDrivers/utils/endianness.hpp>

namespace HyperSonicDrivers::utils
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

    TEST(Endianness, readLE_uint16)
    {
        uint16_t i = 1;
        const void* ptr = &i;
        EXPECT_EQ(readLE_uint16(ptr), swapLE16(i));
    }

    TEST(Endianness, readBE_uint16)
    {
        uint16_t i = 1;
        const void* ptr = &i;
        EXPECT_EQ(readBE_uint16(ptr), swapBE16(i));
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
