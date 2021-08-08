#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <files/ADLFile.hpp>

namespace files
{
    TEST(ADLFile, ADLv1)
    {
        ADLFile f("fixtures/EOBSOUND.ADL");
        EXPECT_EQ(f.getVersion(), 1);
        EXPECT_EQ(f.getNumTracks(), 42);
        EXPECT_EQ(f.getNumInstruments(), 40);
    }

    TEST(ADLFile, ADLv2)
    {
        ADLFile f("fixtures/DUNE19.ADL");
        EXPECT_EQ(f.getVersion(), 2);
        EXPECT_EQ(f.getNumTracks(), 72);
        EXPECT_EQ(f.getNumInstruments(), 71);
    }

    TEST(ADLFile, ADLv3)
    {
        ADLFile f("fixtures/LOREINTR.ADL");
        EXPECT_EQ(f.getVersion(), 3);
        EXPECT_EQ(f.getNumTracks(), 71);
        EXPECT_EQ(f.getNumInstruments(), 134);
    }

    TEST(ADLFile, file_not_valid)
    {
        EXPECT_THROW(ADLFile f("TestFile.exe"), std::invalid_argument);
    }

    TEST(ADLFile, file_not_found)
    {
        EXPECT_THROW(ADLFile f(""), std::system_error);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
