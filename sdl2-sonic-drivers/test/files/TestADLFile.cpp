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
        EXPECT_EQ(f.getNumPrograms(), 150);
        EXPECT_EQ(f.getNumTracks(), 15);
        EXPECT_EQ(f.getNumTrackOffsets(), 42);
        EXPECT_EQ(f.getNumInstrumentOffsets(), 40);
        EXPECT_EQ(f.getDataSize(), 13019);
        EXPECT_EQ(f.getData()[f.getTrackOffset(f.getTrack(2))], 9);
    }

    TEST(ADLFile, ADLv2)
    {
        ADLFile f("fixtures/DUNE19.ADL");
        EXPECT_EQ(f.getVersion(), 2);
        EXPECT_EQ(f.getNumPrograms(), 250);
        EXPECT_EQ(f.getNumTracks(), 49);
        EXPECT_EQ(f.getNumTrackOffsets(), 72);
        EXPECT_EQ(f.getNumInstrumentOffsets(), 71);
        EXPECT_EQ(f.size(), 7257);
        EXPECT_EQ(f.getDataSize(), 7137);
        EXPECT_EQ(f.getTrack(2), 3);
        
        int track2 = f.getTrack(2);
        EXPECT_EQ(track2, 3);
        int track2_offset = f.getTrackOffset(track2);
        EXPECT_EQ(track2_offset, 2877);
        int chan = f.getData()[track2_offset];
        EXPECT_EQ(chan, 9);
    }

    TEST(ADLFile, Dune2LogoAndIntro)
    {
        ADLFile f("fixtures/DUNE0.ADL");
        EXPECT_EQ(f.getVersion(), 2);
        EXPECT_EQ(f.getNumTracks(), 18);
        EXPECT_EQ(f.getNumTrackOffsets(), 52);
        EXPECT_EQ(f.getNumInstrumentOffsets(), 63);
        EXPECT_EQ(f.size(), 14473);
        EXPECT_EQ(f.getDataSize(), 14353);
        EXPECT_EQ(f.getTrack(2), 2);
        EXPECT_EQ(f.getTrackOffset(f.getTrack(2)), 12622);
        EXPECT_EQ(f.getTrack(3), 0xFF);
        //EXPECT_EQ(f.getTrackOffset(f.getTrack(3)), 2282);
        // Logo section
        EXPECT_EQ(f.getTrack(4), 70);
        EXPECT_EQ(f.getTrackOffset(f.getTrack(4)), 13633);
        
        int chan = f.getData()[f.getTrackOffset(f.getTrack(2))];
        EXPECT_EQ(chan , 9);
        chan = f.getData()[f.getTrackOffset(f.getTrack(4))];
        EXPECT_EQ(chan, 9);
    }

    TEST(ADLFile, ADLv3)
    {
        ADLFile f("fixtures/LOREINTR.ADL");
        EXPECT_EQ(f.getVersion(), 3);
        EXPECT_EQ(f.getNumPrograms(), 500);
        EXPECT_EQ(f.getNumTracks(), 30);
        EXPECT_EQ(f.getNumTrackOffsets(), 58);
        EXPECT_EQ(f.getNumInstrumentOffsets(), 71);
        EXPECT_EQ(f.getDataSize(), 13812);

        EXPECT_EQ(f.getData()[f.getTrackOffset(f.getTrack(0))], 9);
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
        EXPECT_THROW(f.getInstrumentOffset(151), std::out_of_range);
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
