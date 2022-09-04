#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include <memory>
#include <drivers/midi/opl/OplVoice.hpp>
#include <drivers/opl/OplWriter.hpp>
#include <hardware/opl/OPL2instrument.h>

namespace hardware
{
    namespace opl
    {
        class OplMock : public OPL
        {

            virtual bool init() override { _init = true; return true; }
            virtual void reset() override {};
            virtual void write(int a, int v) override {};
            virtual uint8_t read(int a) override { return 0; };
            virtual void writeReg(int r, int v) override {};
            virtual void setCallbackFrequency(int timerFrequency) override {}
            virtual void startCallbacks(int timerFrequency) override {};
            virtual void stopCallbacks() override {};
            virtual const std::shared_ptr<audio::scummvm::SoundHandle> getSoundHandle() const noexcept { return nullptr; }
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
                
                EXPECT_FALSE(v.getVibrato());
                EXPECT_FALSE(v.ctrl_modulationWheel(0, 40 + 1));
               
                v.setFree(false);
                EXPECT_TRUE(v.ctrl_modulationWheel(0, 40 + 1));
                EXPECT_TRUE(v.getVibrato());
                EXPECT_TRUE(v.ctrl_modulationWheel(0, 40 - 1));
                EXPECT_FALSE(v.getVibrato());
            }
        }
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
