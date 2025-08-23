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
        EXPECT_EQ(f.getNumTracks(), 120);
        EXPECT_EQ(f.getNumTrackOffsets(), 42);
        EXPECT_EQ(f.getNumInstrumentOffsets(), 40);
        EXPECT_EQ(f.getDataSize(), 13019 - 600);
        EXPECT_EQ(f.getData()[f.getTrackOffset(f.getTrack(2))], 9);
        EXPECT_EQ(f.getProgramOffset(f.getTrack(2), ADLFile::PROG_TYPE::Track), f.getTrackOffset(f.getTrack(2)));
        EXPECT_EQ(f.getProgramOffset(f.getTrack(2), ADLFile::PROG_TYPE::Instrument), f.getInstrumentOffset(f.getTrack(2)));
    }

    TEST(ADLFile, ADLv2)
    {
        ADLFileMock f("../fixtures/DUNE19.ADL");
        EXPECT_EQ(f.getVersion(), 2);
        EXPECT_EQ(f.getNumTracks(), 120);
        EXPECT_EQ(f.getNumTrackOffsets(), 72);
        EXPECT_EQ(f.getNumInstrumentOffsets(), 71);
        EXPECT_EQ(f.parentSize(), 7257);
        EXPECT_EQ(f.getDataSize(), 7137 - 1000);
        EXPECT_EQ(f.getTrack(2), 3);

        EXPECT_EQ(f.getProgramOffset(f.getTrack(2), ADLFile::PROG_TYPE::Track), f.getTrackOffset(f.getTrack(2)));
        EXPECT_EQ(f.getProgramOffset(f.getTrack(2), ADLFile::PROG_TYPE::Instrument), f.getInstrumentOffset(f.getTrack(2)));

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
        EXPECT_EQ(f.getNumTracks(), 120);
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

    /**
     * @brief Tests ADLFile parsing for a version 3 ADL fixture (LOREINTR.ADL).
     *
     * Verifies that the file is recognized as version 3, reports the expected
     * counts (tracks, track offsets, instrument offsets) and data size, and that
     * program offsets for a track resolve to the corresponding track and
     * instrument offsets. Also checks that the first track's data byte equals 9.
     */
    TEST(ADLFile, ADLv3)
    {
        ADLFile f("../fixtures/LOREINTR.ADL");
        EXPECT_EQ(f.getVersion(), 3);
        EXPECT_EQ(f.getNumTracks(), 250);
        EXPECT_EQ(f.getNumTrackOffsets(), 58);
        EXPECT_EQ(f.getNumInstrumentOffsets(), 71);
        EXPECT_EQ(f.getDataSize(), 13812 - 2000);

        EXPECT_EQ(f.getProgramOffset(f.getTrack(2), ADLFile::PROG_TYPE::Track), f.getTrackOffset(f.getTrack(2)));
        EXPECT_EQ(f.getProgramOffset(f.getTrack(2), ADLFile::PROG_TYPE::Instrument), f.getInstrumentOffset(f.getTrack(2)));

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
// -----------------------------------------------------------------------------
// Additional ADL v3 tests (GoogleTest/GoogleMock)
// These tests expand coverage for the LOREINTR.ADL fixture introduced in ADLv3,
// focusing on bounds checking, exception behavior, and offset consistency.
// -----------------------------------------------------------------------------
namespace HyperSonicDrivers::files::westwood
{
    // Verify that every declared track/instrument offset lies within the data buffer.
    TEST(ADLFile, ADLv3_OffsetsWithinBounds_AllEntries)
    {
        ADLFile f("../fixtures/LOREINTR.ADL");
        const auto dataSize = f.getDataSize();

        for (int idx = 0; idx < f.getNumTrackOffsets(); ++idx)
        {
            const auto off = f.getTrackOffset(idx);
            EXPECT_LT(off, dataSize) << "track offset idx=" << idx;
        }

        for (int idx = 0; idx < f.getNumInstrumentOffsets(); ++idx)
        {
            const auto off = f.getInstrumentOffset(idx);
            EXPECT_LT(off, dataSize) << "instrument offset idx=" << idx;
        }
    }

    // Validate exception behavior on out-of-range indices for v3.
    TEST(ADLFile, ADLv3_InvalidIndicesThrow)
    {
        ADLFile f("../fixtures/LOREINTR.ADL");

        // Index equal to size should be invalid for all tables.
        EXPECT_THROW(f.getTrack(f.getNumTracks()), std::out_of_range);
        EXPECT_THROW(f.getTrackOffset(f.getNumTrackOffsets()), std::out_of_range);
        EXPECT_THROW(f.getInstrumentOffset(f.getNumInstrumentOffsets()), std::out_of_range);
    }

    // Track table entries should either be a valid index into the offset tables or the 0xFF sentinel.
    // For valid entries, program offsets must resolve to the corresponding offsets.
    TEST(ADLFile, ADLv3_TrackTableIndicesOrSentinel)
    {
        ADLFile f("../fixtures/LOREINTR.ADL");

        // Probe a spread of entries (covers early, mid, and later indices).
        const int probes[] = {0, 1, 2, 5, 10, 20, 50, 100, 200};
        for (int probe : probes)
        {
            if (probe >= f.getNumTracks())
                break;

            const int trackIndex = f.getTrack(probe);
            if (trackIndex != 0xFF)
            {
                EXPECT_LT(trackIndex, f.getNumTrackOffsets())
                    << "trackIndex " << trackIndex << " not < numTrackOffsets";

                EXPECT_EQ(
                    f.getProgramOffset(trackIndex, ADLFile::PROG_TYPE::Track),
                    f.getTrackOffset(trackIndex)
                );
                EXPECT_EQ(
                    f.getProgramOffset(trackIndex, ADLFile::PROG_TYPE::Instrument),
                    f.getInstrumentOffset(trackIndex)
                );
            }
            else
            {
                // Sentinel entries are allowed; presence itself is acceptable.
                SUCCEED();
            }
        }
    }

    // Parent buffer size should not be smaller than the data section size.
    TEST(ADLFile, ADLv3_ParentSizeAtLeastDataSize)
    {
        ADLFileMock f("../fixtures/LOREINTR.ADL");
        EXPECT_GE(f.parentSize(), f.getDataSize());
    }

    // The first track is used elsewhere; ensure it's not a sentinel and the first byte
    // at the resolved track offset looks like a valid channel nibble (0..15).
    TEST(ADLFile, ADLv3_FirstTrackNotSentinelAndChannelNibbleRange)
    {
        ADLFile f("../fixtures/LOREINTR.ADL");
        const auto t0 = f.getTrack(0);
        ASSERT_NE(t0, 0xFF) << "First track must not be sentinel for this validation";

        const auto off0 = f.getTrackOffset(t0);
        const unsigned char first = static_cast<unsigned char>(f.getData()[off0]);
        EXPECT_LE(static_cast<int>(first), 15) << "First data byte expected within 0..15";
    }
}
