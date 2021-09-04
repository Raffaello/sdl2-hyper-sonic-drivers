#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <files/MIDFile.hpp>
#include <string>
#include <cstdint>
#include <vector>

namespace files
{
    class VLQTest : public ::testing::TestWithParam<std::tuple<std::vector<uint8_t>, uint32_t>>{};
    TEST_P(VLQTest, variable_length_quantities)
    {
        auto values = std::get<0>(GetParam());
        uint32_t exp_value = std::get<1>(GetParam());
        const uint8_t* buf = values.data();
        EXPECT_EQ(MIDFile::decode_VLQ(buf), exp_value);
    }
    INSTANTIATE_TEST_SUITE_P(
        MIDFile,
        VLQTest,
        ::testing::Values(
            std::make_tuple<std::vector<uint8_t>, uint32_t>({ 0 }, 0),
            std::make_tuple<std::vector<uint8_t>, uint32_t>({ 0x40 }, 0x40),
            std::make_tuple<std::vector<uint8_t>, uint32_t>({ 0x7F }, 0x7F),
            std::make_tuple<std::vector<uint8_t>, uint32_t>({ 0x81, 0 }, 0x80),
            std::make_tuple<std::vector<uint8_t>, uint32_t>({ 0xC0, 0 }, 0x2000),
            std::make_tuple<std::vector<uint8_t>, uint32_t>({ 0xFF, 0x7F }, 0x3FFF),
            std::make_tuple<std::vector<uint8_t>, uint32_t>({ 0x81, 0x80, 0 }, 0x4000),
            std::make_tuple<std::vector<uint8_t>, uint32_t>({ 0xC0, 0x80, 0 }, 0x100000),
            std::make_tuple<std::vector<uint8_t>, uint32_t>({ 0xFF, 0xFF, 0x7F }, 0x1FFFFF),
            std::make_tuple<std::vector<uint8_t>, uint32_t>({ 0x81, 0x80, 0x80, 0 }, 0x200000),
            std::make_tuple<std::vector<uint8_t>, uint32_t>({ 0xC0, 0x80, 0x80, 0 }, 0x8000000),
            std::make_tuple<std::vector<uint8_t>, uint32_t>({ 0xFF, 0xFF, 0xFF, 0x7F }, 0x0FFFFFFF)
        )
    );

    TEST(MIDFile, cstorDefault)
    {
        //EXPECT_NO_THROW(MIDFile f("fixtures/MI_intro.mid"));
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
