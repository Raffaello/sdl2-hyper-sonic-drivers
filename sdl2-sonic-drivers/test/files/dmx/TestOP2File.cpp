#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <files/dmx/OP2File.hpp>

namespace files
{
    namespace dmx
    {
        TEST(OP2File, cstorDefault)
        {
            OP2File f("fixtures/GENMIDI.OP2");

            EXPECT_STRCASEEQ("Acoustic Grand Piano", f.getInstrumentName(0).c_str());
            EXPECT_STRCASEEQ("Open Triangle", f.getInstrumentName(OP2FILE_NUM_INSTRUMENTS - 1).c_str());
        }

        TEST(OP2File, file_not_found)
        {
            EXPECT_THROW(OP2File f(""), std::system_error);
        }

        TEST(OP2File, getInstruments_out_of_bound)
        {
            OP2File f("fixtures/GENMIDI.OP2");

            EXPECT_THROW(f.getInstrument(255), std::out_of_range);
        }

        TEST(OP2File, getInstrumentsName_out_of_bound)
        {
            OP2File f("fixtures/GENMIDI.OP2");

            EXPECT_THROW(f.getInstrumentName(255), std::out_of_range);
        }

        TEST(OP2File, grandPiano)
        {
            OP2File f("fixtures/GENMIDI.OP2");
            OP2File::instrument_t expInstr;
            auto instr = f.getInstrument(0);

            expInstr.flags = 0;
            expInstr.fineTune = 0x80;
            expInstr.noteNum = 0;
            expInstr.voices[0].iModChar = 0x30;
            expInstr.voices[0].iModAttack = 0xF0;
            expInstr.voices[0].iModSustain = 0xF3;
            expInstr.voices[0].iModWaveSel = 0x01;

            
            EXPECT_EQ(instr.flags, 0);
            EXPECT_EQ(instr.fineTune, 0x80);
            EXPECT_EQ(instr.noteNum, 0);

            EXPECT_EQ(instr.voices[0].iModChar, 0x30);
            EXPECT_EQ(instr.voices[0].iModAttack, 0xF0);
            EXPECT_EQ(instr.voices[0].iModSustain, 0xF3);
            EXPECT_EQ(instr.voices[0].iModWaveSel, 0x01);
            EXPECT_EQ(instr.voices[0].iModScale, 0x40);
            EXPECT_EQ(instr.voices[0].iModLevel, 0x14);
            EXPECT_EQ(instr.voices[0].iFeedback, 0x0A);
            EXPECT_EQ(instr.voices[0].iCarChar, 0x30);
            EXPECT_EQ(instr.voices[0].iCarAttack, 0xF1);
            EXPECT_EQ(instr.voices[0].iCarSustain, 0xF4);
            EXPECT_EQ(instr.voices[0].iCarWaveSel, 0x01);
            EXPECT_EQ(instr.voices[0].iCarScale, 0x00);
            EXPECT_EQ(instr.voices[0].iCarLevel, 0x00);
            EXPECT_EQ(instr.voices[0].reserved, 0);
            EXPECT_EQ(instr.voices[0].noteOffset, 0);

            EXPECT_EQ(instr.voices[1].iModChar, 0x00);
            EXPECT_EQ(instr.voices[1].iModAttack, 0x00);
            EXPECT_EQ(instr.voices[1].iModSustain, 0xF0);
            EXPECT_EQ(instr.voices[1].iModWaveSel, 0x00);
            EXPECT_EQ(instr.voices[1].iModScale, 0x00);
            EXPECT_EQ(instr.voices[1].iModLevel, 0x00);
            EXPECT_EQ(instr.voices[1].iFeedback, 0x00);
            EXPECT_EQ(instr.voices[1].iCarChar, 0x00);
            EXPECT_EQ(instr.voices[1].iCarAttack, 0x00);
            EXPECT_EQ(instr.voices[1].iCarSustain, 0xF0);
            EXPECT_EQ(instr.voices[1].iCarWaveSel, 0x00);
            EXPECT_EQ(instr.voices[1].iCarScale, 0x00);
            EXPECT_EQ(instr.voices[1].iCarLevel, 0x00);
            EXPECT_EQ(instr.voices[1].reserved, 0);
            EXPECT_EQ(instr.voices[1].noteOffset, 0);
        }
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
