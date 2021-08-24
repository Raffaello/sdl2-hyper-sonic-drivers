#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <hardware/opl/scummvm/mame/mame.hpp>
#include <audio/SDL2Mixer.hpp>
#include <memory>
#include <cstdint>
#include <files/File.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

namespace hardware
{
    namespace opl
    {
        namespace scummvm
        {
            namespace mame
            {
                TEST(OPL, cstorDefault)
                {
                    int rate = 44100;

                    ASSERT_EQ(SDL_Init(SDL_INIT_AUDIO), 0);
                    ASSERT_EQ(Mix_OpenAudio(rate, AUDIO_S16, 2, 1024), 0);

                    std::shared_ptr<audio::SDL2Mixer> mixer = std::make_shared<audio::SDL2Mixer>();
                    EXPECT_EQ(mixer.use_count(), 1);

                    OPL mame(mixer);
                    EXPECT_EQ(mixer.use_count(), 2);
                    EXPECT_EQ(mame.getRate(), rate);
                    EXPECT_EQ(mame.endOfData(), false);
                    EXPECT_EQ(mame.isStereo(), false);

                    Mix_Quit();
                    SDL_Quit();
                }

                TEST(OPL, share_ptrDefault)
                {
                    int rate = 44100;

                    ASSERT_EQ(SDL_Init(SDL_INIT_AUDIO), 0);
                    ASSERT_EQ(Mix_OpenAudio(rate, AUDIO_S16, 2, 1024), 0);

                    std::shared_ptr<audio::SDL2Mixer> mixer = std::make_shared<audio::SDL2Mixer>();
                    EXPECT_EQ(mixer.use_count(), 1);

                    std::shared_ptr<OPL> mame = std::make_shared<OPL>(mixer);
                    EXPECT_EQ(mixer.use_count(), 2);
                    EXPECT_EQ(mame.use_count(), 1);
                    EXPECT_EQ(mame->getRate(), rate);
                    EXPECT_EQ(mame->endOfData(), false);
                    EXPECT_EQ(mame->isStereo(), false);

                    Mix_Quit();
                    SDL_Quit();
                }

                TEST(OPL, Table440Hz)
                {
                    int rate = 22050;

                    ASSERT_EQ(SDL_Init(SDL_INIT_AUDIO), 0);
                    ASSERT_EQ(Mix_OpenAudio(rate, AUDIO_S16, 2, 1024), 0);

                    std::shared_ptr<audio::SDL2Mixer> mixer = std::make_shared<audio::SDL2Mixer>();
                    std::shared_ptr<hardware::opl::scummvm::mame::OPL> opl = std::make_shared<hardware::opl::scummvm::mame::OPL>(mixer);
                    opl->init();
                    opl->setCallbackFrequency(72);
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

                    Mix_Quit();
                    SDL_Quit();
                }
            }
        }
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
