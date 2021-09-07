#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <files/MIDFile.hpp>
#include <string>
#include <cstdint>
#include <vector>
#include <spdlog/spdlog.h>

namespace files
{
    TEST(MIDFile, cstorDefault)
    {
        MIDFile f("fixtures/MI_intro.mid");

        EXPECT_EQ(f.getFormat(), 1);
        EXPECT_EQ(f.getNumTracks(), 15);
        EXPECT_EQ(f.getDivision(), 192);

        MIDFile::MIDI_track_t track0 = f.getTrack(0);
        EXPECT_EQ(track0.events.size(), 4);
        EXPECT_EQ(track0.events[3].type.val, 0xFF);
        EXPECT_EQ(track0.events[3].events.size(), 1);
        EXPECT_EQ(track0.events[3].events[0], (int)MIDFile::MIDI_META_EVENT::END_OF_TRACK);
        EXPECT_EQ(track0.events[3].delta_time, 0);
        //EXPECT_EQ(f.getTotalTime(), 180);
    }

    TEST(MIDFile, file_not_found)
    {
        EXPECT_THROW(MIDFile f(""), std::system_error);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
