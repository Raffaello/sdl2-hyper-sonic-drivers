#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include <memory>
#include <HyperSonicDrivers/drivers/midi/opl/OplVoice.hpp>
#include <HyperSonicDrivers/drivers/opl/OplWriter.hpp>
#include <HyperSonicDrivers/hardware/opl/OPL2instrument.h>
#include <HyperSonicDrivers/files/dmx/OP2File.hpp>
#include <HyperSonicDrivers/hardware/opl/OPLMock.hpp>
#include <HyperSonicDrivers/drivers/midi/IMidiChannel.hpp>

namespace HyperSonicDrivers::drivers::midi::opl
{
    class OplVoiceMock : public OplVoice
    {
    public:
        hardware::opl::OPL2instrument_t* instr;

        OplVoiceMock(const uint8_t slot, const drivers::opl::OplWriter* oplWriter) :
            OplVoice(slot, oplWriter)
        {
            instr = new hardware::opl::OPL2instrument_t();
            memset((void*)instr, 0, sizeof(hardware::opl::OPL2instrument_t));
            setInstrument(instr);
        }

        ~OplVoiceMock()
        {
            delete instr;
        }

        inline void setChannel(IMidiChannel* channel) noexcept { m_channel = channel; }
        inline IMidiChannel* getChannel() const noexcept { return m_channel; };

        void setFree(const bool f) {
            m_free = f;
        }

        inline uint8_t getVolume() const { return m_volume; };
        inline uint8_t getPan() const { return m_channel->pan; };
        inline uint8_t getPitch() const { return m_channel->pitch; };
        inline uint8_t getPitchFactor() const { return m_pitch_factor; };
    };

    TEST(OplVoice, ctrl_modulation_wheel)
    {
        auto opl = std::make_shared<hardware::opl::OPLMock>();
        const bool opl3_mode = false;
        auto ow = std::make_unique < drivers::opl::OplWriter>(opl, opl3_mode);
        IMidiChannel midi_channel(0);

        OplVoiceMock v((uint8_t)0, ow.get());
        v.setChannel(&midi_channel);

        EXPECT_FALSE(v.isVibrato());
        EXPECT_FALSE(v.ctrl_modulationWheel(40 + 1));

        v.setFree(false);
        EXPECT_TRUE(v.ctrl_modulationWheel(40 + 1));
        EXPECT_TRUE(v.isVibrato());
        EXPECT_TRUE(v.ctrl_modulationWheel(40 - 1));
        EXPECT_FALSE(v.isVibrato());
    }

    TEST(OplVoice, allocate)
    {
        files::dmx::OP2File f("../fixtures/GENMIDI.OP2");
        IMidiChannel midi_channel0(0);
        IMidiChannel midi_channel1(1);
        auto b = f.getBank();

        auto opl = std::make_shared<hardware::opl::OPLMock>();
        const bool opl3_mode = false;
        auto ow = std::make_unique < drivers::opl::OplWriter>(opl, opl3_mode);

        OplVoiceMock v1((uint8_t)0, ow.get());
        OplVoiceMock v2((uint8_t)1, ow.get());
        v1.setChannel(&midi_channel0);
        v2.setChannel(&midi_channel0);

        EXPECT_EQ(v1.getChannel(), &midi_channel0);
        EXPECT_EQ(v2.getChannel(), &midi_channel0);

        IMidiChannel* ch = &midi_channel1;
        const uint8_t note = 100;
        const uint8_t vol = 80;
        const uint8_t vol2 = 79;
        //const bool secondary = false;
        const uint8_t ch_mod = 64;
        const uint8_t ch_vol = 100;
        const uint8_t ch_pitch = 16;
        const uint8_t ch_pan = 32;
        ch->modulation = ch_mod;
        ch->volume = ch_vol;
        ch->pitch = ch_pitch;
        ch->pan = ch_pan;

        // A Channel with 2 Voices...
        const uint8_t slot1 = v1.allocate(ch, note, vol, b->getInstrumentPtr(0), false);
        const uint8_t slot2 = v2.allocate(ch, note, vol2, b->getInstrumentPtr(0), true);

        // 1st voice
        EXPECT_FALSE(v1.isFree());
        EXPECT_EQ(v1.getChannel(), ch);

        EXPECT_EQ(v1.getSlot(), 0);
        EXPECT_EQ(v1.getSlot(), slot1);
        EXPECT_EQ(v1.getNote(), note);
        EXPECT_EQ(v1.getVolume(), vol);
        EXPECT_EQ(v1.getChannel()->volume, ch_vol);
        const int cmpInstr1 = memcmp(&b->getInstrumentPtr(0)->voices[0], v1.getInstrument(), sizeof(hardware::opl::OPL2instrument_t));
        EXPECT_EQ(cmpInstr1, 0);
        EXPECT_TRUE(v1.isVibrato());
        EXPECT_EQ(v1.getPitch(), ch_pitch);
        EXPECT_EQ(v1.getPitchFactor(), ch_pitch);
        EXPECT_EQ(v1.getPan(), ch_pan);

        // 2nd voice
        EXPECT_FALSE(v2.isFree());
        EXPECT_EQ(v2.getChannel(), ch);

        EXPECT_EQ(v2.getSlot(), 1);
        EXPECT_EQ(v2.getSlot(), slot2);
        EXPECT_EQ(v2.getNote(), note);
        EXPECT_EQ(v2.getVolume(), vol2);
        EXPECT_EQ(v2.getChannel()->volume, ch_vol);
        const int cmpInstr2 = memcmp(&b->getInstrumentPtr(0)->voices[1], v2.getInstrument(), sizeof(hardware::opl::OPL2instrument_t));
        EXPECT_EQ(cmpInstr2, 0);
        EXPECT_TRUE(v2.isVibrato());
        EXPECT_EQ(v2.getPitch(), ch_pitch);
        EXPECT_EQ(v2.getPitchFactor(), ch_pitch);
        EXPECT_EQ(v2.getPan(), ch_pan);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
