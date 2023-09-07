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

    TEST(VOCFile, cstorDefault)
    {
        EXPECT_NO_THROW(VOCFile f("../fixtures/VSCREAM1.VOC"));
    }

    //TODO: create a parametrized test
    class VocFileTest : public ::testing::TestWithParam<std::tuple<std::string, audio::mixer::eChannelGroup, uint32_t, int, uint8_t>>
    {
    public:
        std::string filename = std::get<0>(GetParam());
        audio::mixer::eChannelGroup group = std::get<1>(GetParam());
        uint32_t freq = std::get<2>(GetParam());
        int dataSize = std::get<3>(GetParam());
        uint8_t data0 = std::get<4>(GetParam());
    };
    TEST_P(VocFileTest, load_file)
    {
        VOCFile f(filename, group);

        EXPECT_STRCASEEQ(f.getVersion().c_str(), "1.10");
        EXPECT_EQ(f.getChannels(), 1);
        EXPECT_EQ(f.getSampleRate(), freq);
        EXPECT_EQ(f.getBitsDepth(), 8);
        EXPECT_EQ(f.getDataSize(), dataSize);
        EXPECT_EQ(f.getData()[0], data0);

        std::shared_ptr<Sound> s = f.getSound();
        EXPECT_EQ(s->bitsDepth, f.getBitsDepth());
        EXPECT_FALSE(s->stereo);
        EXPECT_EQ(s->freq, f.getSampleRate());
        EXPECT_EQ(s->group, group);
    }
    INSTANTIATE_TEST_SUITE_P(
        VOCFile,
        VocFileTest,
        ::testing::Values(
            std::make_tuple("../fixtures/VSCREAM1.VOC", audio::mixer::eChannelGroup::Sfx, 8000, 5817, 0x80),
            std::make_tuple("../fixtures/DUNE.VOC", audio::mixer::eChannelGroup::Speech, 14705, 15233, 0x83)
        )
    );

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
