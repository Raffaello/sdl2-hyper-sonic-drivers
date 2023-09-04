#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <HyperSonicDrivers/files/westwood/ADLFile.hpp>
#include <memory>

int _argc;
char** _argv;

namespace HyperSonicDrivers::files::westwood
{
    class ADLFileMock : public ADLFile
    {
    public:
        ADLFileMock(const std::string& filename) : ADLFile(filename) {};

        auto parentSize() { return size(); };
    };

    TEST(ADLFile, ADLv1)
    {
        ADLFile f("../fixtures/EOBSOUND.ADL");
        EXPECT_EQ(f.getVersion(), 1);
        EXPECT_EQ(f.getNumTracks(), 15);
        EXPECT_EQ(f.getNumTrackOffsets(), 42);
        EXPECT_EQ(f.getNumInstrumentOffsets(), 40);
        EXPECT_EQ(f.getDataSize(), 13019 - 600);
        EXPECT_EQ(f.getData()[f.getTrackOffset(f.getTrack(2))], 9);
        EXPECT_EQ(f.getProgramOffset(f.getTrack(2), ADLFile::PROG_TYPE::TRACK), f.getTrackOffset(f.getTrack(2)));
        EXPECT_EQ(f.getProgramOffset(f.getTrack(2), ADLFile::PROG_TYPE::INSTRUMENT), f.getInstrumentOffset(f.getTrack(2)));
    }

    TEST(ADLFile, ADLv2)
    {
        ADLFileMock f("../fixtures/DUNE19.ADL");
        EXPECT_EQ(f.getVersion(), 2);
        EXPECT_EQ(f.getNumTracks(), 49);
        EXPECT_EQ(f.getNumTrackOffsets(), 72);
        EXPECT_EQ(f.getNumInstrumentOffsets(), 71);
        EXPECT_EQ(f.parentSize(), 7257);
        EXPECT_EQ(f.getDataSize(), 7137 - 1000);
        EXPECT_EQ(f.getTrack(2), 3);

        EXPECT_EQ(f.getProgramOffset(f.getTrack(2), ADLFile::PROG_TYPE::TRACK), f.getTrackOffset(f.getTrack(2)));
        EXPECT_EQ(f.getProgramOffset(f.getTrack(2), ADLFile::PROG_TYPE::INSTRUMENT), f.getInstrumentOffset(f.getTrack(2)));

        int track2 = f.getTrack(2);
        EXPECT_EQ(track2, 3);
        int track2_offset = f.getTrackOffset(track2);
        EXPECT_EQ(track2_offset, 2877 - 1000);
        int chan = f.getData()[track2_offset];
        EXPECT_EQ(chan, 9);
    }

    TEST(ADLFile, Dune2LogoAndIntro)
    {
        ADLFileMock f("../fixtures/DUNE0.ADL");

        EXPECT_EQ(f.getVersion(), 2);
        EXPECT_EQ(f.getNumTracks(), 18);
        EXPECT_EQ(f.getNumTrackOffsets(), 52);
        EXPECT_EQ(f.getNumInstrumentOffsets(), 63);
        EXPECT_EQ(f.parentSize(), 14473);
        EXPECT_EQ(f.getDataSize(), 14353 - 1000);
        EXPECT_EQ(f.getTrack(2), 2);
        EXPECT_EQ(f.getTrackOffset(f.getTrack(2)), 12622 - 1000);
        EXPECT_EQ(f.getTrack(3), 0xFF);
        // Logo section
        EXPECT_EQ(f.getTrack(4), 70);
        EXPECT_EQ(f.getTrackOffset(f.getTrack(4)), 13633 - 1000);

        int chan = f.getData()[f.getTrackOffset(f.getTrack(2))];
        EXPECT_EQ(chan, 9);
        chan = f.getData()[f.getTrackOffset(f.getTrack(4))];
        EXPECT_EQ(chan, 9);
    }

    TEST(ADLFile, ADLv3)
    {
        ADLFile f("../fixtures/LOREINTR.ADL");
        EXPECT_EQ(f.getVersion(), 3);
        EXPECT_EQ(f.getNumTracks(), 30);
        EXPECT_EQ(f.getNumTrackOffsets(), 58);
        EXPECT_EQ(f.getNumInstrumentOffsets(), 71);
        EXPECT_EQ(f.getDataSize(), 13812 - 2000);

        EXPECT_EQ(f.getProgramOffset(f.getTrack(2), ADLFile::PROG_TYPE::TRACK), f.getTrackOffset(f.getTrack(2)));
        EXPECT_EQ(f.getProgramOffset(f.getTrack(2), ADLFile::PROG_TYPE::INSTRUMENT), f.getInstrumentOffset(f.getTrack(2)));

        EXPECT_EQ(f.getData()[f.getTrackOffset(f.getTrack(0))], 9);
    }

    TEST(ADLFile, file_not_valid)
    {
        EXPECT_THROW(ADLFile f(_argv[0]), std::invalid_argument);
    }

    TEST(ADLFile, file_not_found)
    {
        EXPECT_THROW(ADLFile f(""), std::system_error);
    }

    TEST(ADLFile, notValidTrack)
    {
        ADLFile f("../fixtures/EOBSOUND.ADL");
        EXPECT_THROW(f.getTrack(151), std::out_of_range);
    }

    TEST(ADLFile, notValidInstrument)
    {
        ADLFile f("../fixtures/EOBSOUND.ADL");
        EXPECT_THROW(f.getInstrumentOffset(151), std::out_of_range);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    _argc = argc;
    _argv = argv;

    return RUN_ALL_TESTS();
}
