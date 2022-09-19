#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include <memory>
#include <drivers/midi/opl/OplVoice.hpp>
#include <drivers/opl/OplWriter.hpp>
#include <hardware/opl/OPL2instrument.h>
#include <files/dmx/OP2File.hpp>

namespace hardware
{
    namespace opl
    {
        class OplMock : public OPL
        {
        public:
            OplMock(): OPL(OplType::OPL2) {}
            virtual bool init() override { _init = true; return true; }
            virtual void reset() override {};
            virtual void write(int a, int v) override {};
            virtual uint8_t read(int a) override { return 0; };
            virtual void writeReg(int r, int v) override {};
            virtual void setCallbackFrequency(int timerFrequency) override {}
            virtual void startCallbacks(int timerFrequency) override {};
            virtual void stopCallbacks() override {};
            virtual std::shared_ptr<audio::scummvm::SoundHandle> getSoundHandle() const noexcept { return nullptr; }
        };
    }
}

namespace drivers
{
    namespace midi
    {
        namespace opl
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

                void setChannel(const uint8_t ch) {
                    OplVoice::setChannel(ch);
                }

                void setFree(const bool f) {
                    OplVoice::setFree(f);
                }
            };

            TEST(OplVoice, ctrl_modulation_wheel)
            {
                auto opl = std::make_shared<hardware::opl::OplMock>();
                const bool opl3_mode = false;
                auto ow = std::make_unique < drivers::opl::OplWriter>(opl, opl3_mode);
                
                OplVoiceMock v((uint8_t)0, ow.get());
                v.setChannel(0);
                
                EXPECT_FALSE(v.isVibrato());
                EXPECT_FALSE(v.ctrl_modulationWheel(0, 40 + 1));
               
                v.setFree(false);
                EXPECT_TRUE(v.ctrl_modulationWheel(0, 40 + 1));
                EXPECT_TRUE(v.isVibrato());
                EXPECT_TRUE(v.ctrl_modulationWheel(0, 40 - 1));
                EXPECT_FALSE(v.isVibrato());
            }

            TEST(OplVoice, allocate)
            {
                files::dmx::OP2File f("fixtures/GENMIDI.OP2");
                auto b = f.getBank();

                auto opl = std::make_shared<hardware::opl::OplMock>();
                const bool opl3_mode = false;
                auto ow = std::make_unique < drivers::opl::OplWriter>(opl, opl3_mode);

                OplVoiceMock v1((uint8_t)0, ow.get());
                OplVoiceMock v2((uint8_t)1, ow.get());
                v1.setChannel(0);
                v2.setChannel(0);

                EXPECT_EQ(v1.getChannel(), 0);
                EXPECT_EQ(v2.getChannel(), 0);

                const uint8_t ch = 1;
                const uint8_t note = 100;
                const uint8_t vol = 80;
                //const bool secondary = false;
                const uint8_t ch_mod = 64;
                const uint8_t ch_vol = 100;
                const uint8_t ch_pitch = 16;
                const uint8_t ch_pan = 32;

                // A Channel with 2 Voices...
                const uint8_t slot1 = v1.allocate(ch, note, vol, b->getInstrumentPtr(0), false, ch_mod, ch_vol, ch_pitch, ch_pan);
                const uint8_t slot2 = v2.allocate(ch, note, vol, b->getInstrumentPtr(0), true, ch_mod, ch_vol, ch_pitch, ch_pan);

                // 1st voice
                EXPECT_FALSE(v1.isFree());
                EXPECT_TRUE(v1.isChannelBusy(ch));
                EXPECT_FALSE(v1.isChannelFree(ch));
                EXPECT_EQ(v1.getChannel(), ch);

                EXPECT_EQ(v1.getSlot(), 0);
                EXPECT_EQ(v1.getSlot(), slot1);
                EXPECT_EQ(v1.getNote(), note);
                EXPECT_EQ(v1.getVolume(), vol);
                const int cmpInstr1 = memcmp(&b->getInstrumentPtr(0)->voices[0], v1.getInstrument(), sizeof(hardware::opl::OPL2instrument_t));
                EXPECT_EQ(cmpInstr1, 0);
                EXPECT_TRUE(v1.isVibrato());
                EXPECT_EQ(v1.getRealVolume(), ch_vol*vol/127);
                EXPECT_EQ(v1.getPitch(), ch_pitch);
                EXPECT_EQ(v1.getPan(), ch_pan);

                // 2nd voice
                EXPECT_FALSE(v2.isFree());
                EXPECT_TRUE(v2.isChannelBusy(ch));
                EXPECT_FALSE(v2.isChannelFree(ch));
                EXPECT_EQ(v2.getChannel(), ch);

                EXPECT_EQ(v2.getSlot(), 1);
                EXPECT_EQ(v2.getSlot(), slot2);
                EXPECT_EQ(v2.getNote(), note);
                EXPECT_EQ(v2.getVolume(), vol);
                const int cmpInstr2 = memcmp(&b->getInstrumentPtr(0)->voices[1], v2.getInstrument(), sizeof(hardware::opl::OPL2instrument_t));
                EXPECT_EQ(cmpInstr2, 0);
                EXPECT_TRUE(v2.isVibrato());
                EXPECT_EQ(v2.getRealVolume(), ch_vol*vol/127);
                EXPECT_EQ(v2.getPitch(), ch_pitch);
                EXPECT_EQ(v2.getPan(), ch_pan);
            }
        }
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
