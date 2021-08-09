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

    TEST(XMIFile, file_not_valid)
    {
        EXPECT_THROW(XMIFile f("TestXMIFile.exe"), std::invalid_argument);
    }

    TEST(XMIFile, file_not_found)
    {
        EXPECT_THROW(XMIFile f(""), std::system_error);
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

    TEST(XMIFile, headerNoXdirXmi)
    {
        XMIFile f("fixtures/header_no_xdir.xmi");
        EXPECT_EQ(f.size(), 34);
        EXPECT_EQ(f.getNumTracks(), 1);
        auto t = f.getTrack(0);
        EXPECT_EQ(t.size(), 2);
        EXPECT_EQ(t[0], 0xA0);
        EXPECT_EQ(t[1], 0xB0);
    }

    TEST(XMIFile, AIL2_14_DEMOXmi)
    {
        XMIFile f("fixtures/AIL2_14_DEMO.XMI");
        EXPECT_EQ(f.size(), 26950);
        EXPECT_EQ(f.getNumTracks(), 3);
    }

    TEST(XMIFile, full)
    {
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
