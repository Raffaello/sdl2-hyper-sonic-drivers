#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <HyperSonicDrivers/files/dmx/MUSFile.hpp>
#include <HyperSonicDrivers/files/MIDFile.hpp>

namespace HyperSonicDrivers::files::dmx
{
    TEST(MUSFile, cstorDefault)
    {
        MUSFile f("../fixtures/D_E1M1.MUS");

        auto midi = f.getMIDI();
        EXPECT_EQ(midi->format, audio::midi::MIDI_FORMAT::SINGLE_TRACK);
        EXPECT_EQ(midi->numTracks, 1);
        EXPECT_EQ(midi->division, MUSFile::MUS_PLAYBACK_SPEED_DEFAULT / 2);
        auto te = midi->getTrack().getEvents();

        EXPECT_EQ(te.size(), 5828);
    }

    TEST(MUSFile, file_not_found)
    {
        EXPECT_THROW(MUSFile f(""), std::system_error);
    }

    TEST(DISABLED_MUSFile, musfile_sample)
    {
        MUSFile f1("../fixtures/D_E1M1.MUS");
        MIDFile f2("../fixtures/D_E1M1.mid"); // converted from MUS2MID (DOS) v1.0

        auto m1 = f1.getMIDI();
        auto m2 = f2.getMIDI();
        EXPECT_EQ(m1->format, m2->format);
        EXPECT_EQ(m1->division, m2->division);
        EXPECT_EQ(m1->numTracks, m2->numTracks);

        auto te1 = m1->getTrack().getEvents();
        auto te2 = m2->getTrack().getEvents();
        ASSERT_LE(te1.size(), te2.size());

        for (int i = 0; i < te1.size(); ++i)
        {
            auto e1 = te1.at(i);
            auto e2 = te2.at(i);

            EXPECT_EQ(e1.delta_time, e2.delta_time);
            EXPECT_EQ(e1.type.val, e2.type.val);
            ASSERT_EQ(e1.data.size(), e2.data.size());
            for (int j = 0; j < e1.data.size(); j++)
                EXPECT_EQ(e1.data.at(j), e2.data.at(j));
        }
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
