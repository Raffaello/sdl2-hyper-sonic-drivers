#include <gtest/gtest.h>
#include <HyperSonicDrivers/audio/Renderer.hpp>
#include <HyperSonicDrivers/devices/Adlib.hpp>
#include <HyperSonicDrivers/devices/SbPro2.hpp>
#include <HyperSonicDrivers/drivers/westwood/ADLDriver.hpp>
#include <HyperSonicDrivers/audio/mixer/ChannelGroup.hpp>
#include <HyperSonicDrivers/hardware/opl/OplEmulator.hpp>
#include <HyperSonicDrivers/audio/stubs/StubMixer.hpp>
#include <filesystem>
#include <string>

namespace HyperSonicDrivers::audio
{
    using audio::mixer::eChannelGroup;
    using hardware::opl::OplEmulator;
    using devices::eDeviceName;

    class RendererTest : public ::testing::TestWithParam<std::tuple<std::string, const int, const devices::eDeviceName, const OplEmulator>>
    {
    public:
        const std::string test_name = std::get<0>(GetParam());
        const int freq = std::get<1>(GetParam());
        const eDeviceName device_name = std::get<2>(GetParam());
        const OplEmulator opl_emu = std::get<3>(GetParam());

        std::shared_ptr<devices::Opl> opl;
        std::shared_ptr<IMixer> mixer;

        RendererTest()
        {
            mixer = std::make_shared<stubs::StubMixer>();
            switch (device_name)
            {
                using enum devices::eDeviceName;
            case Adlib:
                opl = devices::make_device<devices::Adlib, devices::Opl>(mixer, opl_emu);
                break;
            case SbPro2:
                opl = devices::make_device<devices::SbPro2, devices::Opl>(mixer, opl_emu);
                break;
            default:
                throw std::invalid_argument("");
            }
        }
    };

    TEST_P(RendererTest, render_wav)
    {
        const std::string exp_renderer = "../fixtures/test_renderer_" + test_name + ".wav";
        const std::string rfile = "../fixtures/test_renderer_" + test_name + "_out.wav";

        if (std::filesystem::exists(rfile))
            std::filesystem::remove(rfile);

        ASSERT_FALSE(std::filesystem::exists(rfile));

        {
            audio::Renderer r(freq);
            r.openOutputFile(rfile);

            auto drv1 = drivers::westwood::ADLDriver(opl, eChannelGroup::Music);
            auto af = std::make_shared<files::westwood::ADLFile>("../fixtures/DUNE0.ADL");
            drv1.setADLFile(af);

            drv1.play(4);
            while (drv1.isPlaying())
                r.renderBuffer(opl);

            r.closeOutputFile();
        }

        files::WAVFile w(rfile);
        auto sound = w.getSound();
        files::WAVFile wexp(exp_renderer);
        auto exp_sound = wexp.getSound();

        ASSERT_EQ(sound->dataSize, exp_sound->dataSize);
        ASSERT_EQ(sound->freq, exp_sound->freq);
        ASSERT_EQ(sound->stereo, exp_sound->stereo);
        EXPECT_EQ(sound->freq, freq);
        EXPECT_EQ(sound->stereo, opl->getHardware()->isStereo());
        for (uint32_t i = 0; i < sound->dataSize; i++)
        {
            EXPECT_EQ(sound->data[i], exp_sound->data[i]);
        }
    }
    INSTANTIATE_TEST_SUITE_P(
        Renderer,
        RendererTest,
        ::testing::Values(
            std::make_tuple<>("adlib_mame2", 44100, eDeviceName::Adlib, OplEmulator::MAME),
            std::make_tuple<>("sbpro2_dosbox", 44100, eDeviceName::SbPro2, OplEmulator::DOS_BOX)
        )
    );


    // These 2 disabled test are generating a shorter wav file. WHY?
    TEST(DISABLED_Renderer, adlib_mame2)
    {
        constexpr const char* exp_renderer = "../fixtures/test_renderer_adlib_mame2.wav";
        constexpr const char* rfile = "test_renderer_adlib_mame2_out.wav";

        if (std::filesystem::exists(rfile))
            std::filesystem::remove(rfile);

        ASSERT_FALSE(std::filesystem::exists(rfile));

        audio::Renderer r(1024);
        r.openOutputFile(rfile);

        auto mixer = std::make_shared<stubs::StubMixer>();

        auto adlib = devices::make_device<devices::Adlib, devices::Opl>(mixer, OplEmulator::MAME);
        auto drv1 = drivers::westwood::ADLDriver(adlib, eChannelGroup::Music);
        auto af = std::make_shared<files::westwood::ADLFile>("../fixtures/DUNE0.ADL");
        drv1.setADLFile(af);

        drv1.play(4);
        while (drv1.isPlaying())
            r.renderBuffer(adlib);

        r.closeOutputFile();

        files::WAVFile w(rfile);
        auto sound = w.getSound();
        files::WAVFile wexp(exp_renderer);
        auto exp_sound = wexp.getSound();

        ASSERT_EQ(sound->dataSize, exp_sound->dataSize);
        ASSERT_EQ(sound->freq, exp_sound->freq);
        ASSERT_EQ(sound->stereo, exp_sound->stereo);
        EXPECT_EQ(sound->freq, 44100);
        EXPECT_FALSE(sound->stereo);
        for (uint32_t i = 0; i < sound->dataSize; i++)
        {
            EXPECT_EQ(sound->data[i], exp_sound->data[i]);
        }
    }

    TEST(DISABLED_Renderer, sbpro2_dosbox)
    {
        constexpr const char* exp_renderer = "../fixtures/test_renderer_sbpro2_dosbox.wav";
        constexpr const char* rfile = "test_renderer_sbpro2_dosbox_out.wav";

        if (std::filesystem::exists(rfile))
            std::filesystem::remove(rfile);

        ASSERT_FALSE(std::filesystem::exists(rfile));

        audio::Renderer r(1024);
        r.openOutputFile(rfile);

        auto mixer = std::make_shared<stubs::StubMixer>();

        auto sbpro2 = devices::make_device<devices::SbPro2, devices::Opl>(mixer, OplEmulator::DOS_BOX);
        auto drv1 = drivers::westwood::ADLDriver(sbpro2, eChannelGroup::Music);
        auto af = std::make_shared<files::westwood::ADLFile>("../fixtures/DUNE0.ADL");
        drv1.setADLFile(af);

        drv1.play(4);
        while (drv1.isPlaying())
            r.renderBuffer(sbpro2);

        r.closeOutputFile();

        files::WAVFile w(rfile);
        auto sound = w.getSound();
        files::WAVFile wexp(exp_renderer);
        auto exp_sound = wexp.getSound();

        ASSERT_EQ(sound->dataSize, exp_sound->dataSize);
        ASSERT_EQ(sound->freq, exp_sound->freq);
        ASSERT_EQ(sound->stereo, exp_sound->stereo);
        EXPECT_EQ(sound->freq, 44100);
        EXPECT_TRUE(sound->stereo);
        for (uint32_t i = 0; i < sound->dataSize; i++)
        {
            EXPECT_EQ(sound->data[i], exp_sound->data[i]);
        }
    }

    /*TEST(DISABLED_Renderer, adlib_mame2_device)
    {
        constexpr const char* exp_renderer = "../fixtures/test_renderer_adlib_mame2.wav";
        constexpr const char* rfile = "test_renderer_adlib_mame2_out.wav";

        if (std::filesystem::exists(rfile))
            std::filesystem::remove(rfile);

        ASSERT_FALSE(std::filesystem::exists(rfile));

        audio::Renderer r(1024);
        r.openOutputFile(rfile);

        auto mixer = std::make_shared<stubs::StubMixer>();

        auto adlib = devices::make_device<devices::Adlib, devices::Opl>(mixer, OplEmulator::MAME);
        auto drv1 = drivers::westwood::ADLDriver(adlib, eChannelGroup::Music);
        auto af = std::make_shared<files::westwood::ADLFile>("../fixtures/DUNE0.ADL");
        drv1.setADLFile(af);

        drv1.play(4);
        while (drv1.isPlaying())
            r.renderBuffer(adlib);

        r.closeOutputFile();

        files::WAVFile w(rfile);
        auto sound = w.getSound();
        files::WAVFile wexp(exp_renderer);
        auto exp_sound = wexp.getSound();

        ASSERT_EQ(sound->dataSize, exp_sound->dataSize);
        ASSERT_EQ(sound->freq, exp_sound->freq);
        ASSERT_EQ(sound->stereo, exp_sound->stereo);
        for (uint32_t i = 0; i < sound->dataSize; i++)
        {
            EXPECT_EQ(sound->data[i], exp_sound->data[i]);
        }
    }*/
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
