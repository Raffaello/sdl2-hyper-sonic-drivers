#pragma once

#include <cstdint>
#include <memory>
#include <filesystem>
#include <HyperSonicDrivers/audio/IMixer.hpp>
#include <HyperSonicDrivers/audio/midi/MIDIEvent.hpp>
#include <HyperSonicDrivers/devices/IDevice.hpp>
#include <HyperSonicDrivers/hardware/mt32/MT32.hpp>

namespace HyperSonicDrivers::devices
{
    class MT32 : public IDevice
    {
    public:
        explicit MT32(
            const std::shared_ptr<audio::IMixer>& mixer,
            const std::filesystem::path& control_rom_file,
            const std::filesystem::path& pcm_rom_file
        );
        ~MT32() override = default;

        virtual bool init() noexcept = 0;
        virtual bool shutdown() noexcept = 0;

        inline std::shared_ptr<hardware::mt32::MT32> getMt32() const noexcept { return m_mt32; };
    private:
        std::shared_ptr<hardware::mt32::MT32> m_mt32;
    };
}
