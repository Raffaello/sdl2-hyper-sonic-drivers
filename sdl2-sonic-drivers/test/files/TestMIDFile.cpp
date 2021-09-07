#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <files/MIDFile.hpp>
#include <string>
#include <cstdint>
#include <vector>
#include <spdlog/spdlog.h>

namespace files
{
    class VLQTest : public ::testing::TestWithParam<std::tuple<std::vector<uint8_t>, uint32_t, int>>{};
    TEST_P(VLQTest, variable_length_quantities)
    {
        auto values = std::get<0>(GetParam());
        uint32_t exp_value = std::get<1>(GetParam());
        int exp_reads = std::get<2>(GetParam());
        const uint8_t* buf = values.data();
        uint32_t value = 0;

        int reads = MIDFile::decode_VLQ(buf, value);
        EXPECT_EQ(value, exp_value);
        EXPECT_EQ(reads, exp_reads);
    }
    INSTANTIATE_TEST_SUITE_P(
        MIDFile,
        VLQTest,
        ::testing::Values(
            std::make_tuple<std::vector<uint8_t>, uint32_t, int>({ 0 }, 0, 1),
            std::make_tuple<std::vector<uint8_t>, uint32_t, int>({ 0x40 }, 0x40, 1),
            std::make_tuple<std::vector<uint8_t>, uint32_t, int>({ 0x7F }, 0x7F, 1),
            std::make_tuple<std::vector<uint8_t>, uint32_t, int>({ 0x81, 0 }, 0x80, 2),
            std::make_tuple<std::vector<uint8_t>, uint32_t, int>({ 0xC0, 0 }, 0x2000, 2),
            std::make_tuple<std::vector<uint8_t>, uint32_t, int>({ 0xFF, 0x7F }, 0x3FFF, 2),
            std::make_tuple<std::vector<uint8_t>, uint32_t, int>({ 0x81, 0x80, 0 }, 0x4000, 3),
            std::make_tuple<std::vector<uint8_t>, uint32_t, int>({ 0xC0, 0x80, 0 }, 0x100000, 3),
            std::make_tuple<std::vector<uint8_t>, uint32_t, int>({ 0xFF, 0xFF, 0x7F }, 0x1FFFFF, 3),
            std::make_tuple<std::vector<uint8_t>, uint32_t, int>({ 0x81, 0x80, 0x80, 0 }, 0x200000, 4),
            std::make_tuple<std::vector<uint8_t>, uint32_t, int>({ 0xC0, 0x80, 0x80, 0 }, 0x8000000, 4),
            std::make_tuple<std::vector<uint8_t>, uint32_t, int>({ 0xFF, 0xFF, 0xFF, 0x7F }, 0x0FFFFFFF, 4)
        )
    );

    TEST(MIDFile, cstorDefault)
    {
        MIDFile f("fixtures/MI_intro.mid");

        EXPECT_EQ(f.getFormat(), 1);
        EXPECT_EQ(f.getNumTracks(), 15);
        EXPECT_EQ(f.getDivision(), 192);

        auto track0 = f.getTrack(0);
        EXPECT_EQ(track0.events.size(), 4);
        EXPECT_EQ(track0.events[3].type.val, 0xFF);
        EXPECT_EQ(track0.events[3].events.size(), 1);
        EXPECT_EQ(track0.events[3].events[0], (int)MIDFile::MIDI_META_EVENT::END_OF_TRACK);
        EXPECT_EQ(track0.events[3].delta_time, 0);
        EXPECT_EQ(f.getTotalTime(), 180);
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