#pragma once

#include <cstdint>
//#include <string>
#include <memory>
#include <filesystem>
#include <optional>
#include <mt32emu/c_interface/cpp_interface.h>
#include <HyperSonicDrivers/audio/IMixer.hpp>

namespace HyperSonicDrivers::hardware::mt32
{
    class MT32
    {
    public:
        MT32(const std::filesystem::path& control_rom, const std::filesystem::path& pcm_rom,
            const std::shared_ptr<audio::IMixer>& mixer
        );
        virtual ~MT32();

        inline bool isInit() const noexcept { return m_init; };
        inline bool isStereo() const noexcept { return true; };

        bool init();
        void reset();


        inline std::shared_ptr<audio::IMixer> getMixer() const noexcept { return m_mixer; };
        inline std::optional<uint8_t> getChannelId() const noexcept { return m_channelId; };

    protected:
        bool m_init = false;
        std::shared_ptr<audio::IMixer> m_mixer;
        std::optional<uint8_t> m_channelId;
        std::unique_ptr<uint8_t[]> m_control_rom_data;
        std::unique_ptr<uint8_t[]> m_pcm_rom_data;
        size_t m_control_rom_data_size;
        size_t m_pcm_rom_data_size;
        uint32_t m_output_rate;

        MT32Emu::Service m_service;
    };
}