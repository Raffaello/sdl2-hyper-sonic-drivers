// NOTE: Test framework and library:
// - Preferred: GoogleTest (gtest). This repository's detected setup should be used.
// - If the project uses Catch2/doctest instead, replace the gtest includes/assertions accordingly while keeping the same test logic.
//
// These tests focus on behaviors exposed/altered in the PR diff for IMidiChannelVoice:
//   - getChannelNum()
//   - setVolumes() which updates m_volume and recomputes real volume via calcVolume_().
// The internal calcVolume_() is validated indirectly by observing effective volume changes via public accessors.
// If a direct getter for "real volume" does not exist, we provide a minimal test double deriving from IMidiChannelVoice
// with accessors solely for test visibility, without altering production code paths.

#include <cstdint>
#include <limits>
#include <type_traits>

// Try to include GoogleTest; adjust if your project uses a different framework.
#include <gtest/gtest.h>

#include <HyperSonicDrivers/drivers/midi/IMidiChannelVoice.hpp>
#include <HyperSonicDrivers/drivers/midi/IMidiChannel.hpp>

using HyperSonicDrivers::drivers::midi::IMidiChannelVoice;
using HyperSonicDrivers::drivers::midi::IMidiChannel;

namespace {

// Minimal stub for IMidiChannel if it's an interface; if IMidiChannel is a struct with fields `channel` and `volume`,
// include and use the real one. Where necessary, we define only what tests need and rely on compilation to signal mismatches.
struct TestMidiChannel : public IMidiChannel {
    // We assume IMidiChannel has public members `uint8_t channel; uint8_t volume;`
    // If they are protected/private, adjust to use provided public setters in IMidiChannel.
    // Initialize with defaults for safety.
    TestMidiChannel(uint8_t ch, uint8_t vol) {
        this->channel = ch;
        this->volume = vol;
    }
};

// A thin concrete test double for IMidiChannelVoice to expose volume state for assertions.
// If IMidiChannelVoice is abstract with other pure virtuals, implement them as no-ops for test purposes.
class TestMidiChannelVoice : public IMidiChannelVoice {
public:
    explicit TestMidiChannelVoice(IMidiChannel* ch) {
        m_channel = ch; // accessing protected member if allowed; otherwise use constructor of real class if available.
        m_volume = 127;
        m_real_volume = calcVolume_();
    }

    // Expose state for tests
    uint8_t exposedVolume() const { return m_volume; }
    uint8_t exposedRealVolume() const { return m_real_volume; }

    // IMidiChannelVoice may declare additional pure virtuals; provide trivial implementations as needed.
protected:
    // If IMidiChannelVoice has protected ctor/dtor semantics, ensure this class matches requirements.
    // Any audio rendering or hardware interactions are intentionally omitted in tests.
    using IMidiChannelVoice::calcVolume_; // grant access within this test double
};

} // namespace

// ------------------------------ Tests ------------------------------

TEST(IMidiChannelVoiceBehavior, GetChannelNumReflectsUnderlyingChannel) {
    // Arrange
    TestMidiChannel channel(/*ch*/5, /*vol*/100);
    TestMidiChannelVoice voice(&channel);

    // Act
    auto num = voice.getChannelNum();

    // Assert
    EXPECT_EQ(num, 5u) << "getChannelNum should return IMidiChannel::channel";
}

TEST(IMidiChannelVoiceBehavior, SetVolumesUpdatesStoredVolumeAndRecalculatesRealVolumeWithChannelVolumeScaling) {
    // Arrange
    TestMidiChannel channel(/*ch*/2, /*vol*/64); // 50% channel volume (approx, 64/127)
    TestMidiChannelVoice voice(&channel);

    // Precondition: max volume => real volume equals channel scaling
    // With initial m_volume=127, expected real = min(127 * 64 / 127, 127) = 64
    EXPECT_EQ(voice.exposedRealVolume(), static_cast<uint8_t>(64));

    // Act
    // Set voice volume to 100, expected real = min(100 * 64 / 127, 127)
    voice.setVolumes(100);
    const uint8_t expected = static_cast<uint8_t>(std::min<int>(100 * 64 / 127, 127));

    // Assert
    EXPECT_EQ(voice.exposedVolume(), static_cast<uint8_t>(100));
    EXPECT_EQ(voice.exposedRealVolume(), expected);
}

TEST(IMidiChannelVoiceBehavior, RealVolumeIsClampedTo127OnOverflow) {
    // Arrange: Set channel volume to maximum; set voice volume to maximum to test clamp.
    TestMidiChannel channel(/*ch*/3, /*vol*/127);
    TestMidiChannelVoice voice(&channel);

    // Act
    voice.setVolumes(200); // Although API takes uint8_t, values beyond 127 will wrap; ensure test uses capped within 0..255
    // After implicit cast to uint8_t, 200 -> 200-256=-56 -> 200 mod 256 = 200; semantics depend on implementation.
    // For robustness, test with 255 as extreme inside uint8_t range to verify clamp behavior.
    voice.setVolumes(255);

    // Expected: min(255 * 127 / 127, 127) = min(255, 127) = 127
    EXPECT_EQ(voice.exposedRealVolume(), static_cast<uint8_t>(127));
}

TEST(IMidiChannelVoiceBehavior, ZeroChannelVolumeForcesRealVolumeZeroRegardlessOfVoiceVolume) {
    // Arrange
    TestMidiChannel channel(/*ch*/9, /*vol*/0);
    TestMidiChannelVoice voice(&channel);

    // Act
    voice.setVolumes(0);
    EXPECT_EQ(voice.exposedRealVolume(), static_cast<uint8_t>(0));

    voice.setVolumes(127);
    EXPECT_EQ(voice.exposedRealVolume(), static_cast<uint8_t>(0));

    voice.setVolumes(64);
    EXPECT_EQ(voice.exposedRealVolume(), static_cast<uint8_t>(0));
}

TEST(IMidiChannelVoiceBehavior, ZeroVoiceVolumeProducesZeroRealVolumeForAnyChannelVolume) {
    // Arrange
    TestMidiChannel channel(/*ch*/10, /*vol*/127);
    TestMidiChannelVoice voice(&channel);

    // Act
    voice.setVolumes(0);

    // Assert
    EXPECT_EQ(voice.exposedVolume(), static_cast<uint8_t>(0));
    EXPECT_EQ(voice.exposedRealVolume(), static_cast<uint8_t>(0));
}

TEST(IMidiChannelVoiceBehavior, MidpointScalingMatchesIntegerDivisionRoundingDown) {
    // Arrange: Channel volume 127 (max), voice 63 should yield 63.
    TestMidiChannel channel(/*ch*/4, /*vol*/127);
    TestMidiChannelVoice voice(&channel);

    // Act
    voice.setVolumes(63);

    // Assert
    EXPECT_EQ(voice.exposedRealVolume(), static_cast<uint8_t>(63));

    // Another case: channel=64 (approx 50%), voice=63 -> floor(63*64/127)=31
    TestMidiChannel channel2(/*ch*/4, /*vol*/64);
    TestMidiChannelVoice voice2(&channel2);
    voice2.setVolumes(63);
    EXPECT_EQ(voice2.exposedRealVolume(), static_cast<uint8_t>( (63 * 64) / 127 ));
}