This test folder assumes GoogleTest (gtest) as the unit testing framework.
If the repository instead uses Catch2 or doctest, replace the gtest includes in:
- `HyperSonicDrivers/drivers/midi/IMidiChannelVoice_behavior_test.cpp`
with the project's framework headers and macros, preserving the same test logic and cases.

Coverage focus (based on the PR diff):
- `IMidiChannelVoice::getChannelNum`
- `IMidiChannelVoice::setVolumes` (and its effect on real volume via `calcVolume_`)

These tests validate:
- Correct channel number retrieval
- Volume scaling by channel volume
- Clamping at 127
- Zero behaviors (voice zero, channel zero)
- Integer division rounding behavior at midpoints