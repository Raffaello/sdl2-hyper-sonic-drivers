#include <HyperSonicDrivers/devices/midi/MidiMT32.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>

namespace HyperSonicDrivers::devices::midi
{
    MidiMT32::MidiMT32(
        const std::shared_ptr<audio::IMixer>& mixer,
        const std::filesystem::path& control_rom_file,
        const std::filesystem::path& pcm_rom_file)
    {
        m_mt32 = std::make_shared<hardware::mt32::MT32>(control_rom_file, pcm_rom_file, mixer);
        if (!m_mt32->init())
        {
            utils::throwLogC<std::runtime_error>(std::format("Can't init device MidiMT32"));
        }
    }

    void MidiMT32::sendEvent(const audio::midi::MIDIEvent& e) const noexcept
    {
        m_mt32->m_service.playMsg(e.toUint32());
    }

    void MidiMT32::sendMessage(const uint8_t msg[], const uint8_t size) const noexcept
    {
    }

    void MidiMT32::sendSysEx(const audio::midi::MIDIEvent& e) const noexcept
    {
    }

    void MidiMT32::pause() const noexcept
    {
    }

    void MidiMT32::resume() const noexcept
    {
    }
}
