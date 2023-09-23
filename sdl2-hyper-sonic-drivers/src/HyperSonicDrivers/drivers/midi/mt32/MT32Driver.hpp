#pragma once

#include <HyperSonicDrivers/drivers/midi/IMidiDriver.hpp>

namespace HyperSonicDrivers::drivers::midi::mt32
{
    class MT32Driver : public IMidiDriver
    {
    public:
        // TODO

        bool open(const audio::mixer::eChannelGroup group,
            const uint8_t volume,
            const uint8_t pan) override { return true; };
        void close() override {};

        void send(const audio::midi::MIDIEvent& e) noexcept override {};
        void send(uint32_t msg) override {};
        void send(int8_t channel, uint32_t msg) override {};

        void pause() const noexcept override {};
        void resume() const noexcept override {};
    };
}
