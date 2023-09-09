#include <HyperSonicDrivers/audio/sdl2/Mixer.hpp>
#include <HyperSonicDrivers/hardware/opl/OPL.hpp>
#include <HyperSonicDrivers/hardware/opl/OPLFactory.hpp>
#include <HyperSonicDrivers/utils/algorithms.hpp>
#include <HyperSonicDrivers/utils/opl.hpp>
#include <spdlog/spdlog.h>
#include <fmt/color.h>
#include <memory>
#include <cstdint>
#include <map>
#include <string>

using namespace HyperSonicDrivers;

using hardware::opl::OPLFactory;
using hardware::opl::OplEmulator;
using hardware::opl::OplType;
using utils::delayMillis;

using utils::FMoutput;
using utils::fm;
using utils::Profm1;
using utils::Profm2;

void opl_test(const OplEmulator emu, const OplType type, const std::shared_ptr<audio::IMixer>& mixer)
{
    constexpr auto LEFT = 0x10;
    constexpr auto RIGHT = 0x20;
    constexpr auto KEYON = 0x20; // key-on bit in regs b0 - b8

    int block;
    int m;
    int f;
    int b;
    int fn;

    auto opl = OPLFactory::create(emu, type, mixer);
    if (opl == nullptr)
        return;
    
    if (!opl->init())
        return;
    opl->start(nullptr);

    /* must initialize this to zero */
    fm(1, 0, opl);

    if (type == OplType::OPL3)
    {
        /* set to OPL3 mode, necessary for stereo */
        Profm2(5, 1, opl);
        /* set both channels, parallel connection */
        fm(0xC0, LEFT | RIGHT | 1, opl);
    }
    else
    {
        /* parallel connection */
        fm(0xC0, 1, opl);
    }

   /***************************************
    * Set parameters for the carrier cell *
    ***************************************/
    /* no amplitude modulation (D7=0), no vibrato (D6=0),
     * sustained envelope type (D5=1), KSR=0 (D4=0),
     * frequency multiplier=1 (D4-D0=1)
     */
    fm(0x23, 0x21, opl);
    /* no volume decrease with pitch (D7-D6=0),
     * no attenuation (D5-D0=0)
     */
    fm(0x43, 0x0, opl);
    /* fast attack (D7-D4=0xF) and decay (D3-D0=0xF) */
    fm(0x63, 0xff, opl);
    /* high sustain level (D7-D4=0), slow release rate (D3-D0=5) */
    fm(0x83, 0x05, opl);

    /*****************************************
     * Set parameters for the modulator cell *
     *****************************************/
     /* sustained envelope type, frequency multiplier=0    */
    fm(0x20, 0x20, opl);
    /* maximum attenuation, no volume decrease with pitch */
    fm(0x40, 0x3f, opl);

    /* Since the modulator signal is attenuated as much as possible, these
     * next two values shouldn't have any effect.
     */
     /* slow attack and decay */
    fm(0x60, 0x44, opl);
    /* high sustain level, slow release rate */
    fm(0x80, 0x05, opl);

    /*************************************************
     * Generate tone from values looked up in table. *
     *************************************************/

    spdlog::info("440 Hz tone, values looked up in table.");
    fm(0xa0, 0x41, opl);  /* 440 Hz */
    fm(0xb0, 0x32, opl);  /* 440 Hz, block 0, key on */

    delayMillis(1000);

    fm(0xb0, 0x12, opl);  /* key off */


    /******************************************
     * Generate tone from a calculated value. *
     ******************************************/

    spdlog::info("440 Hz tone, values calculated.");
    block = 4;         /* choose block=4 and m=1 */
    m = 1;             /* m is the frequency multiple number */
    f = 440;           /* want f=440 Hz */
    b = 1 << block;

    /* This is the equation to calculate frequency number from frequency. */
    fn = (long)f * 1048576 / b / m / 50000L;

    fm(0x23, 0x20 | (m & 0xF), opl);   /* 0x20 sets sustained envelope, low nibble
                                        * is multiple number
                                        */
    fm(0xA0, (fn & 0xFF), opl);
    fm(0xB0, static_cast<uint8_t>(((fn >> 8) & 0x3) + (block << 2) | KEYON), opl);

    delayMillis(1000);

    /*********************************************************
     * Generate a range of octaves by changing block number. *
     *********************************************************/

    spdlog::info("Range of frequencies created by changing block number.");
    for (block = 0; block <= 7; block++) {
        spdlog::info("f={:5d} Hz", (long)440 * (1 << block) / 16);
        fm(0xB0, static_cast<uint8_t>(((fn >> 8) & 0x3) + (block << 2) | KEYON), opl);
        delayMillis(200);
    }


    /*****************************************************************
     * Generate a range of frequencies by changing frequency number. *
     *****************************************************************/

    spdlog::info("Range of frequencies created by changing frequency number.");
    block = 4;
    for (fn = 0; fn < 1024; fn++) {
        fm(0xA0, (fn & 0xFF), opl);
        fm(0xB0, static_cast<uint8_t>(((fn >> 8) & 0x3) + (block << 2) | KEYON), opl);
        delayMillis(1);
    }

    delayMillis(1000);
    /********************************************************************
     * Single tone again.  Both channels, then if on stereo board,      *
     * play tone in just the left channel, then just the right channel. *
     ********************************************************************/

    spdlog::info("440 Hz again, both channels.");
    block = 4;
    fn = 577;                /* This number makes 440 Hz when block=4 and m=1 */
    fm(0xA0, (fn & 0xFF), opl);
    fm(0xB0, static_cast<uint8_t>(((fn >> 8) & 0x3) + (block << 2) | KEYON), opl);
    delayMillis(1000);

    if (type != OplType::OPL2)
    {
        if (type == OplType::OPL3)
        {
            /* This left and right channel stuff is the only part of this program
             * that uses OPL3 mode.  Everything else is available on the OPL2.
             */
            spdlog::info("Left channel only");
            fm(0xC0, LEFT | 1, opl);      /* set left channel only, parallel connection */
            delayMillis(1000);
            
            spdlog::info("Right channel only");
            fm(0xC0, RIGHT | 1, opl);     /* set right channel only, parallel connection */
            delayMillis(1000);
        }
        else
        {

            fm(0xB0, static_cast<uint8_t>(((fn >> 8) & 0x3) + (block << 2)), opl);       // key off
            delayMillis(1000);

            spdlog::info("Left channel only");
            Profm1(0xB0, static_cast<uint8_t>(((fn >> 8) & 0x3) + (block << 2) | KEYON), opl);
            delayMillis(1000);
            
            Profm1(0xB0, static_cast<uint8_t>(((fn >> 8) & 0x3) + (block << 2)), opl);   // key off
            delayMillis(1000);

            spdlog::info("Right channel only");
            Profm2(0xB0, static_cast<uint8_t>(((fn >> 8) & 0x3) + (block << 2) | KEYON), opl);
            delayMillis(1000);
        }
    }

    /*********************************
     * Attenuate the signal by 3 dB. *
     *********************************/
    delayMillis(1000);
    fm(0xB0, static_cast<uint8_t>(((fn >> 8) & 0x3) + (block << 2) | KEYON), opl);
    spdlog::info("Attenuated by 3 dB.");
    fm(0x43, 4, opl);     /* attenuate by 3 dB */
    delayMillis(1000);
    fm(0xB0, static_cast<uint8_t>(((fn >> 8) & 0x3) + (block << 2)), opl);
    delayMillis(1000);

    if (type == OplType::OPL3)
    {
        /* Set OPL-3 back to OPL-2 mode, because if the next program to run was
         * written for the OPL-2, then it won't set the LEFT and RIGHT bits to
         * one, so no sound will be heard.
         */
        Profm2(5, 0, opl);   /* set back to OPL2 mode */
    }

    opl->stop();
}

bool detect_opl2(const OplEmulator emu, const OplType type, std::shared_ptr<audio::IMixer> mixer)
{
    auto opl = OPLFactory::create(emu, type, mixer);
    if (opl == nullptr)
        return false;

    if (!opl->init())
        return false;

    opl->start(nullptr);

    return utils::detectOPL2(opl);
}

bool detect_opl3(const OplEmulator emu, const OplType type, std::shared_ptr<audio::IMixer> mixer)
{
    // Detect OPL2. If present, continue.
    auto opl = OPLFactory::create(emu, type, mixer);
    if (opl == nullptr)
        return false;

    if (!opl->init())
        return false;

    opl->start(nullptr);

    return utils::detectOPL3(opl);
}

int main(int argc, char* argv[])
{
    auto mixer = audio::make_mixer<audio::sdl2::Mixer>(8, 44100, 1024);

    std::map<OplEmulator, std::string> emus = {
        { OplEmulator::DOS_BOX, "DOS_BOX" },
        { OplEmulator::MAME, "MAME" },
        { OplEmulator::NUKED, "NUKED" },
        { OplEmulator::WOODY, "WOODY" },
    };

    std::map<OplType, std::string> types = {
        {OplType::OPL2, "OPL2"},
        {OplType::DUAL_OPL2, "DUAL_OPL2"},
        {OplType::OPL3, "OPL3"},
    };

    std::string m = "##### {} {} #####";
    for (auto& emu : emus)
    {
        for (auto& type : types)
        {
            for (auto& c : { fmt::color::white_smoke, fmt::color::yellow,      fmt::color::aqua,
                             fmt::color::lime_green,  fmt::color::blue_violet, fmt::color::indian_red }) {
                spdlog::info(fmt::format(fg(c), m, emu.second, type.second));
            }
            // by default all are OPL2 compatible,
            // so this value should be always be true.
            bool opl2 = detect_opl2(emu.first, type.first, mixer);
            // only OPL3 should be true, maybe DUAL_OPL2 as well?
            bool opl3 = detect_opl3(emu.first, type.first, mixer);

            std::string msg;

            msg = fmt::format("detect opl2: {}", opl2);
            if (opl2) spdlog::info(msg);
            else spdlog::error(msg);

            msg = fmt::format("detect opl3: {}", opl3);
            if (type.first == OplType::OPL3)
            {
                if (opl3) spdlog::info(msg);
                else spdlog::error(msg);
            }
            else {
                if (!opl3) spdlog::info(msg);
                else spdlog::error(msg);
            }

            try
            {
                opl_test(emu.first, type.first, mixer);
            }
            catch (const std::exception& e)
            {
                spdlog::default_logger()->error(e.what());
            }
        }
    }

    return 0;
}
