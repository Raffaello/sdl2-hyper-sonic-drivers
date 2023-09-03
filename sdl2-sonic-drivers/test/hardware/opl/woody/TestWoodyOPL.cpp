#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <hardware/opl/woody/WoodyOPL.hpp>
#include "../test/audio/stubs/StubMixer.hpp"
#include <memory>
#include <cstdint>
#include <files/File.hpp>

namespace HyperSonicDrivers::hardware::opl::woody
{
    using audio::stubs::StubMixer;

    TEST(OPL, cstorDefaultFalse)
    {
        std::shared_ptr<StubMixer> mixer = std::make_shared<StubMixer>();
        EXPECT_EQ(mixer.use_count(), 1);
        WoodyOPL opl(mixer, false);
        EXPECT_EQ(mixer.use_count(), 2);
        EXPECT_EQ(opl.getRate(), mixer->rate);
        EXPECT_EQ(opl.endOfData(), false);
        EXPECT_EQ(opl.isStereo(), false);
    }

    TEST(OPL, cstorDefaultTrue)
    {
        std::shared_ptr<StubMixer> mixer = std::make_shared<StubMixer>();
        EXPECT_EQ(mixer.use_count(), 1);
        WoodyOPL opl(mixer, true);
        EXPECT_EQ(mixer.use_count(), 2);
        EXPECT_EQ(opl.getRate(), mixer->rate);
        EXPECT_EQ(opl.endOfData(), false);
        EXPECT_EQ(opl.isStereo(), true);
    }

    TEST(OPL, share_ptrDefault)
    {
        std::shared_ptr<StubMixer> mixer = std::make_shared<StubMixer>();
        EXPECT_EQ(mixer.use_count(), 1);

        std::shared_ptr<WoodyOPL> opl = std::make_shared<woody::WoodyOPL>(mixer, false);
        EXPECT_EQ(mixer.use_count(), 2);
        EXPECT_EQ(opl.use_count(), 1);
        EXPECT_EQ(opl->getRate(), mixer->rate);
        EXPECT_EQ(opl->endOfData(), false);
        EXPECT_EQ(opl->isStereo(), false);
    }

    TEST(DISABLED_OPL, Table440Hz)
    {
        std::shared_ptr<StubMixer> mixer = std::make_shared<StubMixer>();
        mixer->rate = 22050;
        std::shared_ptr<WoodyOPL> opl = std::make_shared<WoodyOPL>(mixer, false);
        opl->init();
        opl->setCallbackFrequency(72);

        opl->writeReg(1, 0);        /* must initialize this to zero */
        opl->writeReg(0xC0, 1);     /* parallel connection */
        /****************************************
         *Set parameters for the carrier cell*
         ***************************************/
        opl->writeReg(0x23, 0x21); /* no amplitude modulation (D7=0), no vibrato (D6=0),
                                    * sustained envelope type (D5=1), KSR=0 (D4=0),
                                    * frequency multiplier=1 (D4-D0=1)
                                    */

        opl->writeReg(0x43, 0x0);   /* no volume decrease with pitch (D7-D6=0),
                                     * no attenuation (D5-D0=0)
                                     */

        opl->writeReg(0x63, 0xff);  /* fast attack (D7-D4=0xF) and decay (D3-D0=0xF) */
        opl->writeReg(0x83, 0x05);  /* high sustain level (D7-D4=0), slow release rate (D3-D0=5) */

        /*****************************************
         * Set parameters for the modulator cell *
         *****************************************/

        opl->writeReg(0x20, 0x20);  /* sustained envelope type, frequency multiplier=0    */
        opl->writeReg(0x40, 0x3f);  /* maximum attenuation, no volume decrease with pitch */

        /* Since the modulator signal is attenuated as much as possible, these
         * next two values shouldn't have any effect.
         */
        opl->writeReg(0x60, 0x44);  /* slow attack and decay */
        opl->writeReg(0x80, 0x05);  /* high sustain level, slow release rate */

        /*************************************************
         * Generate tone from values looked up in table. *
         *************************************************/

        opl->writeReg(0xa0, 0x41);  /* 440 Hz */
        opl->writeReg(0xb0, 0x32);  /* 440 Hz, block 0, key on */

        const int len = 4096;
        auto buf = std::make_unique<int16_t[]>(len);
        opl->readBuffer(buf.get(), len / 2);

        files::File f("fixtures/mame-22050-16bit-440Hz.dat");
        EXPECT_EQ(f.size(), 4096);
        auto fbuf = std::make_unique<int16_t[]>(len);
        f.read(fbuf.get(), len);
        for (int i = 0; i < len / 2; i++) {
            EXPECT_EQ(buf[i], fbuf[i]);
        }
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
