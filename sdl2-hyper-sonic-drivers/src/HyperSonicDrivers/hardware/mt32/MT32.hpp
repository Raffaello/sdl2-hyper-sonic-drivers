#pragma once

#include <cstdint>
//#include <string>
#include <memory>
#include <filesystem>
#include <optional>
#include <mt32emu/c_interface/cpp_interface.h>
#include <HyperSonicDrivers/audio/IMixer.hpp>
#include <HyperSonicDrivers/audio/mixer/ChannelGroup.hpp>
#include <HyperSonicDrivers/hardware/types.hpp>


namespace HyperSonicDrivers::devices::midi
{
    class MidiMT32;
}

namespace HyperSonicDrivers::hardware::mt32
{
    // TODO: create a "hardware interface" among MT32 and OPL
    //  in common: isInit, isStereo, init, reset, getMixer, getChannelId,
    //  etc...
    class MT32
    {
        friend devices::midi::MidiMT32;

    public:
        MT32(const std::filesystem::path& control_rom, const std::filesystem::path& pcm_rom,
            const std::shared_ptr<audio::IMixer>& mixer
        );
        virtual ~MT32();

        inline bool isInit() const noexcept { return m_init; };
        inline bool isStereo() const noexcept { return true; };

        bool init();
        void reset();

        /**
        * Start the OPL with callbacks.
        */
        void start(
            const std::shared_ptr<TimerCallBack>& callback,
            const audio::mixer::eChannelGroup group = audio::mixer::eChannelGroup::Plain,
            const uint8_t volume = 255,
            const uint8_t pan = 0,
            const int timerFrequency = 32000);

        /**
         * Stop the OPL
         */
        void stop();

        inline std::shared_ptr<audio::IMixer> getMixer() const noexcept { return m_mixer; };
        inline std::optional<uint8_t> getChannelId() const noexcept { return m_channelId; };

    private:
        bool m_init = false;
        std::shared_ptr<audio::IMixer> m_mixer;
        std::optional<uint8_t> m_channelId;
        uint32_t m_output_rate = 0;

        MT32Emu::Service m_service;
    };
}