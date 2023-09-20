#pragma once

#include <cstdint>
#include <memory>
#include <filesystem>
#include <HyperSonicDrivers/audio/IMixer.hpp>
#include <HyperSonicDrivers/audio/midi/MIDIEvent.hpp>
#include <HyperSonicDrivers/devices/IMidiDevice.hpp>
#include <HyperSonicDrivers/hardware/mt32/MT32.hpp>

namespace HyperSonicDrivers::devices::midi
{
    class MidiMT32 : public IMidiDevice
    {
    public:
        explicit MidiMT32(
            const std::shared_ptr<audio::IMixer>& mixer,
            const std::filesystem::path& control_rom_file,
            const std::filesystem::path& pcm_rom_file
        );
        ~MidiMT32() override = default;

        void sendEvent(const audio::midi::MIDIEvent& e) const noexcept override;
        void sendMessage(const uint8_t msg[], const uint8_t size) const noexcept override;
        void sendSysEx(const audio::midi::MIDIEvent& e) const noexcept override;
        void pause() const noexcept override;
        void resume() const noexcept override;

    private:
        std::shared_ptr<hardware::mt32::MT32> m_mt32;
    };
}
