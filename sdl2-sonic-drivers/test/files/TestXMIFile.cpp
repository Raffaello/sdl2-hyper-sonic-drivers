#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <files/XMIFile.hpp>

namespace files
{
    TEST(XMIFile, cstorDefault)
    {
        XMIFile f("fixtures/header.xmi");
        EXPECT_EQ(f.size(), 56);
        EXPECT_EQ(f.getNumTracks(), 1);
    }

    TEST(XMIFile, headerXmi)
    {
        XMIFile f("fixtures/header.xmi");
        EXPECT_EQ(f.size(), 56);
        EXPECT_EQ(f.getNumTracks(), 1);
        auto t = f.getTrack(0);
        EXPECT_EQ(t.size(), 2);
        EXPECT_EQ(t[0], 0xA0);
        EXPECT_EQ(t[1], 0xB0);
    }

    TEST(XMIFile, file_not_valid)
    {
        EXPECT_THROW(XMIFile f("TestFile.exe"), std::invalid_argument);
    }

    TEST(XMIFile, file_not_found)
    {
        EXPECT_THROW(XMIFile f(""), std::system_error);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
