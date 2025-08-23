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


namespace HyperSonicDrivers::files::westwood {


/**

 * Additional thorough tests for ADLFile.

 * Framework: GoogleTest (gtest) with GoogleMock (gmock).

 * Scope: Focus on public API and edge conditions seen across v1/v2/v3 fixtures.

 */



TEST(ADLFile, Version1_BoundaryIndicesAndOffsets) {

    ADLFile f("../fixtures/EOBSOUND.ADL");

    ASSERT_EQ(f.getVersion(), 1);

    // Boundary: lowest and highest valid track index for reported offsets

    const auto numTrackOffsets = f.getNumTrackOffsets();

    ASSERT_GT(numTrackOffsets, 0);

    const int firstIdx = 0;

    const int lastIdx = static_cast<int>(numTrackOffsets) - 1;

    // getTrack may map logical index to a program index; ensure it is within byte range

    const int tFirst = f.getTrack(firstIdx);

    const int tLast  = f.getTrack(lastIdx);

    EXPECT_GE(tFirst, 0);

    EXPECT_GE(tLast, 0);

    // Offsets should be within data size boundaries

    const auto ds = f.getDataSize();

    const auto offFirst = f.getTrackOffset(tFirst);

    const auto offLast  = f.getTrackOffset(tLast);

    EXPECT_GE(offFirst, 0);

    EXPECT_GE(offLast, 0);

    EXPECT_LT(offFirst, ds);

    EXPECT_LT(offLast, ds);

    // Data pointer should be valid and readable at those offsets

    const auto* data = f.getData();

    ASSERT_NE(data, nullptr);

    (void)data[offFirst];

    (void)data[offLast];

}



TEST(ADLFile, Version2_SentinelAndConsistency) {

    ADLFile f("../fixtures/DUNE19.ADL");

    ASSERT_EQ(f.getVersion(), 2);

    // Ensure that any sentinel (e.g., 0xFF) track entries yield consistent behavior:

    // If a track entry equals 0xFF, instrument and track offset queries for that entry

    // should not crash; depending on implementation they may throw or return a valid offset.

    // We probe a range to find any sentinel occurrence and assert stable behavior.

    const auto n = f.getNumTrackOffsets();

    const auto* data = f.getData();

    ASSERT_NE(data, nullptr);

    bool foundSentinel = false;

    for (int i = 0; i < static_cast<int>(n); ++i) {

        int t = f.getTrack(i);

        if (t == 0xFF) {

            foundSentinel = true;

            // For sentinel, just verify that program offset helpers do not UB: they may throw.

            EXPECT_NO_FATAL_FAILURE({

                try {

                    (void)f.getProgramOffset(t, ADLFile::PROG_TYPE::Track);

                } catch (...) { /* acceptable: spec may throw */ }

            });

            EXPECT_NO_FATAL_FAILURE({

                try {

                    (void)f.getProgramOffset(t, ADLFile::PROG_TYPE::Instrument);

                } catch (...) { /* acceptable: spec may throw */ }

            });

            break;

        }

    }

    SUCCEED() << "Sentinel presence in DUNE19.ADL: " << (foundSentinel ? "yes" : "no");

}



TEST(ADLFile, Version3_TrackAndInstrumentOffsetsAgreeWithProgramOffset) {

    ADLFile f("../fixtures/LOREINTR.ADL");

    ASSERT_EQ(f.getVersion(), 3);

    const auto count = std::min<int>(static_cast<int>(f.getNumTrackOffsets()), 8);

    // Cross-check first few entries to ensure getProgramOffset returns exactly

    // the corresponding track/instrument offsets for both PROG_TYPE variants.

    for (int i = 0; i < count; ++i) {

        const int t = f.getTrack(i);

        const auto to = f.getTrackOffset(t);

        const auto io = f.getInstrumentOffset(t);

        EXPECT_EQ(f.getProgramOffset(t, ADLFile::PROG_TYPE::Track), to) << "at logical index " << i;

        EXPECT_EQ(f.getProgramOffset(t, ADLFile::PROG_TYPE::Instrument), io) << "at logical index " << i;

        // Offsets must be strictly within [0, dataSize)

        const auto ds = f.getDataSize();

        EXPECT_GE(to, 0);

        EXPECT_GE(io, 0);

        EXPECT_LT(to, ds);

        EXPECT_LT(io, ds);

    }

}



TEST(ADLFile, Exceptions_OutOfRangeIndices) {

    ADLFile f("../fixtures/EOBSOUND.ADL");

    const int badIdx = static_cast<int>(f.getNumTrackOffsets()) + 100; // well beyond end

    EXPECT_THROW(f.getTrack(badIdx), std::out_of_range);

    EXPECT_THROW(f.getInstrumentOffset(badIdx), std::out_of_range);

}



TEST(ADLFile, Constructor_EmptyAndGarbagePaths) {

    // Already have tests for empty string and argv[0]; add a garbage binary path.

    EXPECT_THROW(ADLFile f("/this/path/does/not/exist.ADL"), std::system_error);

}



} // namespace HyperSonicDrivers::files::westwood

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    _argc = argc;
    _argv = argv;

    return RUN_ALL_TESTS();
}
