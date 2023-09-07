#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <HyperSonicDrivers/files/VOCFile.hpp>
#include <HyperSonicDrivers/audio/Sound.hpp>
#include <HyperSonicDrivers/audio/mixer/ChannelGroup.hpp>
#include <cstdint>
#include <memory>

namespace HyperSonicDrivers::files
{
    using audio::Sound;
    using audio::scummvm::Mixer;

    TEST(VOCFile, cstorDefault)
    {
        EXPECT_NO_THROW(VOCFile f("../fixtures/VSCREAM1.VOC"));
    }

    //TODO: create a parametrized test

    TEST(VOCFile, VSCREAM1_VOC)
    {
        VOCFile f("../fixtures/VSCREAM1.VOC", audio::mixer::eChannelGroup::Sfx);

        EXPECT_STRCASEEQ(f.getVersion().c_str(), "1.10");
        EXPECT_EQ(f.getChannels(), 1);
        EXPECT_EQ(f.getSampleRate(), 8000);
        EXPECT_EQ(f.getBitsDepth(), 8);
        EXPECT_EQ(f.getDataSize(), 5817);
        EXPECT_EQ(f.getData()[0], 0x80);
        
        std::shared_ptr<Sound> s = f.getSound();
        EXPECT_EQ(s->bitsDepth, f.getBitsDepth());
        EXPECT_FALSE(s->stereo);
        EXPECT_EQ(s->freq, f.getSampleRate());
        EXPECT_EQ(s->group, audio::mixer::eChannelGroup::Sfx);
    }

    TEST(VOCFile, DUNE_VOC)
    {
        VOCFile f("../fixtures/DUNE.VOC", audio::mixer::eChannelGroup::Speech);

        EXPECT_STRCASEEQ(f.getVersion().c_str(), "1.10");
        EXPECT_EQ(f.getChannels(), 1);
        EXPECT_EQ(f.getSampleRate(), 14705);
        EXPECT_EQ(f.getBitsDepth(), 8);
        EXPECT_EQ(f.getDataSize(), 15233);
        EXPECT_EQ(f.getData()[0], 0x83);

        std::shared_ptr<Sound> s = f.getSound();
        EXPECT_EQ(s->bitsDepth, f.getBitsDepth());
        EXPECT_FALSE(s->stereo);
        EXPECT_EQ(s->freq, f.getSampleRate());
        EXPECT_EQ(s->group, audio::mixer::eChannelGroup::Speech);
    }


    TEST(VOCFile, file_not_found)
    {
        EXPECT_THROW(VOCFile f(""), std::system_error);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
