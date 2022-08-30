#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <files/dmx/OP2File.hpp>
#include <audio/opl/banks/OP2Bank.h>
#include <memory>

namespace files
{
    namespace dmx
    {
        using audio::opl::banks::OP2BANK_NUM_INSTRUMENTS;
        using audio::opl::banks::OP2BANK_INSTRUMENT_NUM_VOICES;
        using audio::opl::banks::Op2BankInstrument_t;

        TEST(OP2File, cstorDefault)
        {
            OP2File f("fixtures/GENMIDI.OP2");

            EXPECT_STRCASEEQ("Acoustic Grand Piano", f.getInstrumentName(0).c_str());
            EXPECT_STRCASEEQ("Open Triangle", f.getInstrumentName(OP2BANK_NUM_INSTRUMENTS - 1).c_str());
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

        void expectInstrumentZero(Op2BankInstrument_t& instr)
        {
            EXPECT_EQ(instr.flags, 0);
            EXPECT_EQ(instr.fineTune, 0x80);
            EXPECT_EQ(instr.noteNum, 0);

            EXPECT_EQ(instr.voices[0].trem_vibr_1, 0x30);
            EXPECT_EQ(instr.voices[0].att_dec_1, 0xF0);
            EXPECT_EQ(instr.voices[0].sust_rel_1, 0xF3);
            EXPECT_EQ(instr.voices[0].wave_1, 0x01);
            EXPECT_EQ(instr.voices[0].scale_1, 0x40);
            EXPECT_EQ(instr.voices[0].level_1, 0x14);
            EXPECT_EQ(instr.voices[0].feedback, 0x0A);
            EXPECT_EQ(instr.voices[0].trem_vibr_2, 0x30);
            EXPECT_EQ(instr.voices[0].att_dec_2, 0xF1);
            EXPECT_EQ(instr.voices[0].sust_rel_2, 0xF4);
            EXPECT_EQ(instr.voices[0].wave_2, 0x01);
            EXPECT_EQ(instr.voices[0].scale_2, 0x00);
            EXPECT_EQ(instr.voices[0].level_2, 0x00);
            EXPECT_EQ(instr.voices[0].unused, 0);
            EXPECT_EQ(instr.voices[0].basenote, 0);

            EXPECT_EQ(instr.voices[1].trem_vibr_1, 0x00);
            EXPECT_EQ(instr.voices[1].att_dec_1, 0x00);
            EXPECT_EQ(instr.voices[1].sust_rel_1, 0xF0);
            EXPECT_EQ(instr.voices[1].wave_1, 0x00);
            EXPECT_EQ(instr.voices[1].scale_1, 0x00);
            EXPECT_EQ(instr.voices[1].level_1, 0x00);
            EXPECT_EQ(instr.voices[1].feedback, 0x00);
            EXPECT_EQ(instr.voices[1].trem_vibr_2, 0x00);
            EXPECT_EQ(instr.voices[1].att_dec_2, 0x00);
            EXPECT_EQ(instr.voices[1].sust_rel_2, 0xF0);
            EXPECT_EQ(instr.voices[1].wave_2, 0x00);
            EXPECT_EQ(instr.voices[1].scale_2, 0x00);
            EXPECT_EQ(instr.voices[1].level_2, 0x00);
            EXPECT_EQ(instr.voices[1].unused, 0);
            EXPECT_EQ(instr.voices[1].basenote, 0);
        }

        TEST(OP2File, getBank_check_immutability)
        {
            OP2File f("fixtures/GENMIDI.OP2");

            auto b1 = f.getBank();
            b1->getInstrumentName(0) = "b1";
            b1->getInstrument(0) = b1->getInstrument(2);
            auto b2 = f.getBank();
            
            EXPECT_STRCASENE(b1->getInstrumentName(0).c_str(), "b1");
            EXPECT_STRCASEEQ(b2->getInstrumentName(0).c_str(), "Acoustic Grand Piano");

            EXPECT_NE(b1->getInstrument(0).fineTune, 255);
            EXPECT_EQ(b2->getInstrument(0).fineTune, 0x80);

            expectInstrumentZero(b2->getInstrument(0));
            expectInstrumentZero(b1->getInstrument(0));
        }

        TEST(OP2File, grandPiano)
        {
            OP2File f("fixtures/GENMIDI.OP2");
            Op2BankInstrument_t expInstr;

            expectInstrumentZero(f.getInstrument(0));
        }
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
