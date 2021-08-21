#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <files/ADLFile.hpp>
#include <memory>

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
        EXPECT_EQ(f.getNumTracks(), 58);
        EXPECT_EQ(f.getNumInstruments(), 71);
    }

    TEST(ADLFile, file_not_valid)
    {
        EXPECT_THROW(ADLFile f("TestADLFile.exe"), std::invalid_argument);
    }

    TEST(ADLFile, file_not_found)
    {
        EXPECT_THROW(ADLFile f(""), std::system_error);
    }

    TEST(ADLFile, notValidTrack)
    {
        ADLFile f("fixtures/EOBSOUND.ADL");
        EXPECT_THROW(f.getTrack(151), std::out_of_range);
    }

    TEST(ADLFile, notValidInstrument)
    {
        ADLFile f("fixtures/EOBSOUND.ADL");
        EXPECT_THROW(f.getInstrument(151), std::out_of_range);
    }

    TEST(ADLFile, shared_ptr)
    {
        std::shared_ptr<files::ADLFile> adlFile = std::make_shared<files::ADLFile>("fixtures/DUNE19.ADL");
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
