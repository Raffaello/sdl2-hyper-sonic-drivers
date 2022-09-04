#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include <memory>
#include <vector>
#include <string>
#include <audio/midi/MIDIEvent.hpp>
#include <audio/midi/MIDITrack.hpp>
#include <audio/midi/types.hpp>
#include <audio/scummvm/Mixer.hpp>
#include "../audio/stubs/StubMixer.hpp"
#include <drivers/MIDDriver.hpp>
#include <drivers/midi/Device.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/ostream_sink.h>
#include <sstream>
#include <algorithm>


namespace drivers
{

    namespace midi
    {
        namespace devices
        {
            class SpyDevice : public Device
            {
                virtual void sendEvent(const audio::midi::MIDIEvent& e) const noexcept override
                {

                };
                virtual void sendMessage(const uint8_t msg[], const uint8_t size) const noexcept override
                {

                }
                virtual void sendSysEx(const audio::midi::MIDIEvent& e) const noexcept override
                {

                }
                virtual void pause() const noexcept override
                {

                }
                virtual void resume() const noexcept override
                {

                }
            };
        }
    }

    class MIDDriverMock : public MIDDriver
    {
    public:
        explicit MIDDriverMock(const std::shared_ptr<audio::scummvm::Mixer>& mixer, const std::shared_ptr<midi::Device>& device) :
            MIDDriver(mixer, device)
        {

        }

        void protected_processTrack(const audio::midi::MIDITrack& track, const uint16_t division)
        {
            processTrack(track, division);
        }
    };

    TEST(MIDDriver, SEQUENCE_NAME_META_EVENT)
    {
        using audio::stubs::StubMixer;
        using audio::midi::MIDIEvent;
        using audio::midi::MIDI_EVENT_TYPES_HIGH;
        using audio::midi::MIDI_META_EVENT_TYPES_LOW;
        using audio::midi::MIDI_META_EVENT;

        auto mixer = std::make_shared<StubMixer>();
        auto device = std::make_shared<midi::devices::SpyDevice>();

        // capture spdlog output
        std::ostringstream _oss;
        auto ostream_logger = spdlog::get("gtest_logger");
        if (!ostream_logger)
        {
            auto ostream_sink = std::make_shared<spdlog::sinks::ostream_sink_st>(_oss);
            ostream_logger = std::make_shared<spdlog::logger>("gtest_logger", ostream_sink);
            ostream_logger->set_pattern(">%v<");
            ostream_logger->set_level(spdlog::level::debug);
        }
        spdlog::set_default_logger(ostream_logger);
        // ---

        MIDIEvent e;
        e.delta_time = 0;
        e.type.high = (uint8_t)MIDI_EVENT_TYPES_HIGH::META_SYSEX;
        e.type.low = (uint8_t)MIDI_META_EVENT_TYPES_LOW::META;
        
        // this can be a parameter
        e.data.push_back((uint8_t)MIDI_META_EVENT::SEQUENCE_NAME);

        std::string s = "sequence_name";
        e.data.insert(e.data.end(),s.begin(), s.end());

        auto midi_track = audio::midi::MIDITrack();
        midi_track.addEvent(e);
        
        MIDDriverMock middrv(mixer, device);
        middrv.protected_processTrack(midi_track, 0);

        // retrieve spdlog output
        std::string output = _oss.str();
        
        EXPECT_NE(_oss.str().find("SEQUENCE NAME: " + s), std::string::npos);
        
        // TODO the spdlog output capture shouold be encapsulated in a class with a SetUp method (and teardown)
        _oss.clear();
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
