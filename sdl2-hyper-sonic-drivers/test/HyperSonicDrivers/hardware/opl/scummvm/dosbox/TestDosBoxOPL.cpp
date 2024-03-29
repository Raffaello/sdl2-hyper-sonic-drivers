#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include <memory>
#include <HyperSonicDrivers/hardware/opl/scummvm/dosbox/DosBoxOPL.hpp>
#include <HyperSonicDrivers/audio/IMixerMock.hpp>
#include <HyperSonicDrivers/files/File.hpp>

namespace HyperSonicDrivers::hardware::opl::scummvm::dosbox
{
    // TODO: refactor these tests as they are the same of the one in mame.

    using audio::IMixerMock;

    class OPLType : public  ::testing::TestWithParam<std::tuple<OplType, bool>>
    {
    public:
        OplType opl_type = std::get<0>(GetParam());
        bool isStereo = std::get<1>(GetParam());
    };
    TEST_P(OPLType, cstorDefault)
    {
        std::shared_ptr<IMixerMock> mixer = std::make_shared<IMixerMock>();
        EXPECT_EQ(mixer.use_count(), 1);
        DosBoxOPL dosbox(this->opl_type, mixer);
        EXPECT_EQ(mixer.use_count(), 2);
        ASSERT_TRUE(dosbox.init());
        //EXPECT_EQ(dosbox.getRate(), mixer->rate);
        //EXPECT_EQ(dosbox.endOfData(), false);
        EXPECT_EQ(dosbox.isStereo(), this->isStereo);
    }

    TEST_P(OPLType, share_ptrDefault)
    {

        std::shared_ptr<IMixerMock> mixer = std::make_shared<IMixerMock>();
        EXPECT_EQ(mixer.use_count(), 1);

        std::shared_ptr<DosBoxOPL> dosbox = std::make_shared<DosBoxOPL>(this->opl_type, mixer);
        EXPECT_EQ(mixer.use_count(), 2);
        EXPECT_EQ(dosbox.use_count(), 1);
        ASSERT_TRUE(dosbox->init());
        //EXPECT_EQ(dosbox->getRate(), mixer->rate);
        //EXPECT_EQ(dosbox->endOfData(), false);
        EXPECT_EQ(dosbox->isStereo(), this->isStereo);
    }

    INSTANTIATE_TEST_SUITE_P(
        DosBoxOPL,
        OPLType,
        ::testing::Values(
            std::make_tuple<>(OplType::OPL2, false),
            std::make_tuple<>(OplType::DUAL_OPL2, true),
            std::make_tuple<>(OplType::OPL3, true)
        )
    );

    TEST(DISABLED_OPL, Table440Hz)
    {
        std::shared_ptr<IMixerMock> mixer = std::make_shared<IMixerMock>();
        mixer->rate = 22050;
        std::shared_ptr<hardware::opl::scummvm::dosbox::DosBoxOPL> opl = std::make_shared<hardware::opl::scummvm::dosbox::DosBoxOPL>(OplType::OPL2, mixer);
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
        //opl->readBuffer(buf.get(), len / 2);

        files::File f("../fixtures/dosbox-22050-16bit-440Hz.dat");
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
