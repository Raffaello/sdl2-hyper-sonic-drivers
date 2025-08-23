#include <cstdint>
#include <limits>
#include <type_traits>

// Prefer the project's established test framework.
// Defaulting to GoogleTest. If your project uses Catch2 or doctest,
// replace the includes and TEST/EXPECT macros accordingly.
#include <gtest/gtest.h>

#include <HyperSonicDrivers/drivers/midi/IMidiChannelVoice.hpp>
#include <HyperSonicDrivers/drivers/midi/IMidiChannel.hpp>

namespace HSD = HyperSonicDrivers::drivers::midi;

// A minimal concrete test double for IMidiChannelVoice in case the interface
// has other pure virtual functions. We expose accessors to observe
// effective (real) volume for assertions, relying only on base class behavior.
class TestMidiChannelVoice : public HSD::IMidiChannelVoice {
public:
    // Provide a constructor that sets the channel pointer and initial volume state if needed.
    explicit TestMidiChannelVoice(HSD::IMidiChannel* ch) {
        m_channel = ch;
        // Ensure deterministic initial values
        m_volume = 0;
        m_real_volume = 0;
    }

    // If IMidiChannelVoice declares other pure virtuals, stub them here.
    // For example:
    // void noteOn(uint8_t, uint8_t) override {}
    // void noteOff(uint8_t, uint8_t) override {}
    // ... (No-ops for unit testing of base functionality.)

    // Test-only helpers to read protected/private state computed by base logic.
    uint8_t test_getRealVolume() const noexcept { return m_real_volume; }
    uint8_t test_getRawVolume() const noexcept { return m_volume; }
};

static_assert(std::is_same_v<uint8_t, unsigned char> || std::numeric_limits<uint8_t>::digits == 8,
              "Assumption: uint8_t is 8-bit.");

TEST(IMidiChannelVoiceTests, GetChannelNumReflectsUnderlyingChannel) {
    HSD::IMidiChannel channel{};
    channel.channel = 9;     // Typical percussion channel in MIDI (0-based 9 == channel 10)
    channel.volume = 100;    // Arbitrary

    TestMidiChannelVoice voice(&channel);
    EXPECT_EQ(voice.getChannelNum(), 9u);

    // Mutate underlying channel object and ensure reflection
    channel.channel = 2;
    EXPECT_EQ(voice.getChannelNum(), 2u);
}

TEST(IMidiChannelVoiceTests, SetVolumesUpdatesRawAndRealVolume_WithChannelAtMax127) {
    HSD::IMidiChannel channel{};
    channel.channel = 1;
    channel.volume = 127; // Max channel volume

    TestMidiChannelVoice voice(&channel);

    // When channel volume = 127, real volume should equal requested volume, clamped to 127.
    voice.setVolumes(0);
    EXPECT_EQ(voice.test_getRawVolume(), 0u);
    EXPECT_EQ(voice.test_getRealVolume(), 0u);

    voice.setVolumes(64);
    EXPECT_EQ(voice.test_getRawVolume(), 64u);
    EXPECT_EQ(voice.test_getRealVolume(), 64u);

    voice.setVolumes(127);
    EXPECT_EQ(voice.test_getRawVolume(), 127u);
    EXPECT_EQ(voice.test_getRealVolume(), 127u);

    // Values >127 can appear due to uint8_t domain (0..255). Base code clamps via std::min(..., 127).
    voice.setVolumes(static_cast<uint8_t>(200));
    EXPECT_EQ(voice.test_getRawVolume(), static_cast<uint8_t>(200));
    EXPECT_EQ(voice.test_getRealVolume(), 127u) << "Should saturate at 127";
}

TEST(IMidiChannelVoiceTests, RealVolumeScalesByChannelVolume_AndUsesIntegerDivision) {
    HSD::IMidiChannel channel{};
    channel.channel = 3;
    // Use a mid channel volume to test scaling and truncation
    channel.volume = 64; // approx 50% of 127

    TestMidiChannelVoice voice(&channel);

    // raw=64, channel=64 => (64 * 64) / 127 = 4096/127 = 32 (integer truncation)
    voice.setVolumes(64);
    EXPECT_EQ(voice.test_getRawVolume(), 64u);
    EXPECT_EQ(voice.test_getRealVolume(), 32u) << "Expected truncated integer division result";

    // raw=127, channel=64 => (127 * 64) / 127 = 64
    voice.setVolumes(127);
    EXPECT_EQ(voice.test_getRealVolume(), 64u);

    // raw=1, channel=64 => (1 * 64) / 127 = 0
    voice.setVolumes(1);
    EXPECT_EQ(voice.test_getRealVolume(), 0u) << "Small products should truncate to zero";
}

TEST(IMidiChannelVoiceTests, RealVolumeBecomesZeroWhenChannelVolumeIsZero) {
    HSD::IMidiChannel channel{};
    channel.channel = 7;
    channel.volume = 0;

    TestMidiChannelVoice voice(&channel);

    for (uint16_t v = 0; v <= 255; ++v) {
        voice.setVolumes(static_cast<uint8_t>(v));
        EXPECT_EQ(voice.test_getRealVolume(), 0u) << "Channel volume zero should mute all";
    }
}

TEST(IMidiChannelVoiceTests, SaturatesAt127ForLargeProducts) {
    HSD::IMidiChannel channel{};
    channel.channel = 4;
    channel.volume = 127;

    TestMidiChannelVoice voice(&channel);

    // Any raw volume >= 127 should clamp to 127 when channel volume is max
    for (uint16_t v = 127; v <= 255; ++v) {
        voice.setVolumes(static_cast<uint8_t>(v));
        EXPECT_EQ(voice.test_getRealVolume(), 127u);
    }

    // Also verify saturation when channel volume < 127 but product still exceeds 127.
    channel.volume = 100; // Change the same channel instance (voice references it)
    // raw=200, ch=100 -> 20000/127 = 157 (floor) -> min(157, 127) = 127
    voice.setVolumes(static_cast<uint8_t>(200));
    EXPECT_EQ(voice.test_getRealVolume(), 127u);
}

TEST(IMidiChannelVoiceTests, ChangingChannelVolumeRecalculatesOnNextSetVolumesCall) {
    HSD::IMidiChannel channel{};
    channel.channel = 5;
    channel.volume = 100;

    TestMidiChannelVoice voice(&channel);

    voice.setVolumes(50); // 50*100/127 = 39
    EXPECT_EQ(voice.test_getRealVolume(), 39u);

    // Increase channel volume; real volume should only update when setVolumes is called again
    channel.volume = 127;
    // Real volume remains unchanged until we call setVolumes (contract inferred from impl)
    EXPECT_EQ(voice.test_getRealVolume(), 39u);

    // Trigger recompute by setting the same value again
    voice.setVolumes(50);
    EXPECT_EQ(voice.test_getRealVolume(), 50u);
}

//
// Notes:
// - Testing library/framework: GoogleTest (gtest) assumed based on conventional C++ repos.
//   Please adapt includes/macros if your repository uses Catch2 (TEST_CASE/REQUIRE) or doctest (TEST_CASE/CHECK).
// - External dependencies are not invoked; this is a pure computation test.
// - We use a minimal derived class to expose computed internal state for validation,
//   avoiding white-box access to private members in production code.
//