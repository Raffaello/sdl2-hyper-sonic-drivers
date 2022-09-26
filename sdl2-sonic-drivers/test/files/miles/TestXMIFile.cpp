#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <files/miles/XMIFile.hpp>

int _argc;
char** _argv;

namespace files::miles
{
    TEST(XMIFile, file_not_valid)
    {
        EXPECT_THROW(XMIFile f(_argv[0]), std::invalid_argument);
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
        EXPECT_EQ(t.getEvents().size(), 1);
        EXPECT_EQ(t.getEvents()[0].type.val, 0xFF);
    }

    TEST(XMIFile, headerNoXdirXmi)
    {
        XMIFile f("fixtures/header_no_xdir.xmi");

        EXPECT_EQ(f.getMIDI()->numTracks, 1);
        EXPECT_EQ(f.getMIDI()->format, audio::midi::MIDI_FORMAT::SINGLE_TRACK);
        auto t = f.getMIDI()->getTrack(0);
        EXPECT_EQ(t.getEvents().size(), 1);
        EXPECT_EQ(t.getEvents()[0].type.val, 0xFF);
    }

    TEST(XMIFile, AIL2_14_DEMOXmi)
    {
        XMIFile f("fixtures/AIL2_14_DEMO.XMI");
        EXPECT_EQ(f.getMIDI()->numTracks, 3);

        EXPECT_EQ(f.getMIDI()->format, audio::midi::MIDI_FORMAT::MULTI_TRACK);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    _argc = argc;
    _argv = argv;

    return RUN_ALL_TESTS();
}
