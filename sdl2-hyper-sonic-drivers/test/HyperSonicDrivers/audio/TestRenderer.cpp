#include <gtest/gtest.h>
#include <HyperSonicDrivers/audio/Renderer.hpp>
#include <HyperSonicDrivers/devices/Adlib.hpp>
#include <HyperSonicDrivers/drivers/westwood/ADLDriver.hpp>
#include <HyperSonicDrivers/audio/mixer/ChannelGroup.hpp>
#include <HyperSonicDrivers/hardware/opl/OplEmulator.hpp>
#include <HyperSonicDrivers/audio/stubs/StubMixer.hpp>
#include <filesystem>

namespace HyperSonicDrivers::audio
{
    using audio::mixer::eChannelGroup;
    using devices::Adlib;
    using hardware::opl::OplEmulator;

    TEST(Renderer, adlib_mame2)
    {
        constexpr const char* exp_renderer = "../fixtures/test_renderer_adlib_mame2.wav";
        constexpr const char* rfile = "test_renderer_adlib_mame2_out.wav";

        if (std::filesystem::exists(rfile))
            std::filesystem::remove(rfile);

        ASSERT_FALSE(std::filesystem::exists(rfile));

        audio::Renderer r;
        r.setOutputFile(rfile);

        auto mixer = std::make_shared<stubs::StubMixer>();

        auto adlib = devices::make_device<devices::Adlib, devices::Opl>(mixer, OplEmulator::MAME);
        auto drv1 = drivers::westwood::ADLDriver(adlib, eChannelGroup::Music);
        auto af = std::make_shared<files::westwood::ADLFile>("../fixtures/DUNE0.ADL");
        drv1.setADLFile(af);

        auto eo = adlib->getOpl();
        drv1.play(4);
        while (drv1.isPlaying())
            r.renderBuffer(eo);

        r.releaseOutputFile();

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
    }

    TEST(Renderer, adlib_mame2_device)
    {
        constexpr const char* exp_renderer = "../fixtures/test_renderer_adlib_mame2.wav";
        constexpr const char* rfile = "test_renderer_adlib_mame2_out.wav";

        if (std::filesystem::exists(rfile))
            std::filesystem::remove(rfile);

        ASSERT_FALSE(std::filesystem::exists(rfile));

        audio::Renderer r;
        r.setOutputFile(rfile);

        auto mixer = std::make_shared<stubs::StubMixer>();

        auto adlib = devices::make_device<devices::Adlib, devices::Opl>(mixer, OplEmulator::MAME);
        auto drv1 = drivers::westwood::ADLDriver(adlib, eChannelGroup::Music);
        auto af = std::make_shared<files::westwood::ADLFile>("../fixtures/DUNE0.ADL");
        drv1.setADLFile(af);

        drv1.play(4);
        while (drv1.isPlaying())
            r.renderBuffer(adlib);

        r.releaseOutputFile();

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
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
