#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <files/miles/XMIFile.hpp>

namespace files
{
    namespace miles
    {
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
            
            EXPECT_EQ(f.getMIDI()->numTracks, 1);
            EXPECT_EQ(f.getMIDI()->format, audio::midi::MIDI_FORMAT::SINGLE_TRACK);
            auto t = f.getMIDI()->getTrack(0);
            EXPECT_EQ(t.events.size(), 2);
            EXPECT_EQ(t.events[0].type.val, 0xA0);
            EXPECT_EQ(t.events[1].type.val, 0xB0);
        }

        TEST(XMIFile, headerNoXdirXmi)
        {
            XMIFile f("fixtures/header_no_xdir.xmi");
            
            EXPECT_EQ(f.getMIDI()->numTracks, 1);
            EXPECT_EQ(f.getMIDI()->format, audio::midi::MIDI_FORMAT::SINGLE_TRACK);
            auto t = f.getMIDI()->getTrack(0);
            EXPECT_EQ(t.events.size(), 2);
            EXPECT_EQ(t.events[0].type.val, 0xA0);
            EXPECT_EQ(t.events[1].type.val, 0xB0);
        }

        TEST(XMIFile, AIL2_14_DEMOXmi)
        {
            XMIFile f("fixtures/AIL2_14_DEMO.XMI");
            EXPECT_EQ(f.getMIDI()->numTracks, 3);
            EXPECT_EQ(f.getMIDI()->format, audio::midi::MIDI_FORMAT::MULTI_TRACK);
        }
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
